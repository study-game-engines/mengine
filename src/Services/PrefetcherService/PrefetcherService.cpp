#include "PrefetcherService.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "ThreadTaskPrefetchImageDecoder.h"
#include "ThreadTaskPrefetchSoundDecoder.h"
#include "ThreadTaskPrefetchDataflow.h"
#include "ThreadTaskPrefetchStream.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/Stringstream.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PrefetcherService, Mengine::PrefetcherService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PrefetcherService::PrefetcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PrefetcherService::~PrefetcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::_availableService() const
    {
        bool Engine_PrefetcherServiceAvailable = CONFIG_VALUE( "Engine", "PrefetcherServiceAvailable", true );

        if( Engine_PrefetcherServiceAvailable == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::_initializeService()
    {
        uint32_t PrefetcherService_ThreadCount = CONFIG_VALUE( "PrefetcherService", "ThreadCount", 2 );
        uint32_t PrefetcherService_PacketSize = CONFIG_VALUE( "PrefetcherService", "PacketSize", 64 );

        m_threadQueue = THREAD_SERVICE()
            ->createTaskQueue( PrefetcherService_PacketSize, MENGINE_DOCUMENT_FACTORABLE );

        for( uint32_t index = 0; index != PrefetcherService_ThreadCount; ++index )
        {
            Stringstream ss;
            ss << "ThreadPrefetcherService_" << index;
            ConstString threadName = Helper::stringizeString( ss.str() );

            THREAD_SERVICE()
                ->createThreadProcessor( threadName, ETP_BELOW_NORMAL, MENGINE_DOCUMENT_FACTORABLE );

            m_threads.emplace_back( threadName );

            m_threadQueue->addThread( threadName );
        }

        m_factoryThreadTaskPrefetchImageDecoder = Helper::makeFactoryPool<ThreadTaskPrefetchImageDecoder, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadTaskPrefetchSoundDecoder = Helper::makeFactoryPool<ThreadTaskPrefetchSoundDecoder, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadTaskPrefetchDataflow = Helper::makeFactoryPool<ThreadTaskPrefetchDataflow, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadTaskPrefetchStream = Helper::makeFactoryPool<ThreadTaskPrefetchStream, 16>( MENGINE_DOCUMENT_FACTORABLE );

        m_factoryPrefetchReceiver = Helper::makeFactoryPool<PrefetchReceiver, 256>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetcherService::_finalizeService()
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadTaskPrefetchImageDecoder );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadTaskPrefetchSoundDecoder );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadTaskPrefetchDataflow );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadTaskPrefetchStream );

        m_factoryThreadTaskPrefetchImageDecoder = nullptr;
        m_factoryThreadTaskPrefetchSoundDecoder = nullptr;
        m_factoryThreadTaskPrefetchDataflow = nullptr;
        m_factoryThreadTaskPrefetchStream = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPrefetchReceiver );

        m_factoryPrefetchReceiver = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetcherService::_stopService()
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        m_threads.clear();

        for( const HashtablePrefetchReceiver::value_type & value : m_prefetchReceivers )
        {
            const PrefetchReceiverPtr & receiver = value.element;

            receiver->finalize();
        }

        m_prefetchReceivers.clear();

        for( const ConstString & threadName : m_threads )
        {
            THREAD_SERVICE()
                ->destroyThreadProcessor( threadName );
        }

        if( m_threadQueue != nullptr )
        {
            THREAD_SERVICE()
                ->cancelTaskQueue( m_threadQueue );

            m_threadQueue = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::prefetchImageDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const PrefetcherObserverInterfacePtr & _observer )
    {
        if( this->isAvailableService() == false )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        if( m_threadQueue == nullptr )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        PrefetchReceiverPtr receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == true )
        {
            receiver->acquire();

            _observer->onPrefetchAlreadyExist();

            return true;
        }

        ThreadTaskPrefetchImageDecoderPtr task = m_factoryThreadTaskPrefetchImageDecoder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->initialize( _fileGroup, _filePath, _observer );
        task->setCodecType( _codecType );

        PrefetchReceiverPtr new_receiver = m_factoryPrefetchReceiver->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( new_receiver->initialize( task ) == false )
        {
            return false;
        }
        
        const ConstString & fileGroupName = _fileGroup->getName();

        m_prefetchReceivers.emplace( fileGroupName, _filePath, new_receiver );

        m_threadQueue->addTask( task );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::getImageDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ImageDecoderInterfacePtr * const _decoder )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        PrefetchReceiverPtr receiver;
        if( this->getPrefetch_( _fileGroup, _filePath, &receiver ) == false )
        {
            return false;
        }

        const ThreadTaskPrefetchPtr & prefetcher = receiver->getPrefetcher();

        ThreadTaskPrefetchImageDecoderPtr prefetcherImageDecoder = ThreadTaskPrefetchImageDecoderPtr::from( prefetcher );

        const ImageDecoderInterfacePtr & prefetch_decoder = prefetcherImageDecoder->getDecoder();

        if( prefetch_decoder == nullptr )
        {
            return false;
        }

        *_decoder = prefetch_decoder;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::prefetchSoundDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const PrefetcherObserverInterfacePtr & _observer )
    {
        if( this->isAvailableService() == false )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        if( m_threadQueue == nullptr )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        PrefetchReceiverPtr receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == true )
        {
            receiver->acquire();

            _observer->onPrefetchAlreadyExist();

            return true;
        }

        ThreadTaskPrefetchSoundDecoderPtr task = m_factoryThreadTaskPrefetchSoundDecoder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->initialize( _fileGroup, _filePath, _observer );
        task->setSoundCodec( _codecType );

        PrefetchReceiverPtr new_receiver = m_factoryPrefetchReceiver->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( new_receiver->initialize( task ) == false )
        {
            return false;
        }

        const ConstString & fileGroupName = _fileGroup->getName();

        m_prefetchReceivers.emplace( fileGroupName, _filePath, new_receiver );

        m_threadQueue->addTask( task );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::getSoundDecoder( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, SoundDecoderInterfacePtr * const _decoder )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        PrefetchReceiverPtr receiver;
        if( this->getPrefetch_( _fileGroup, _filePath, &receiver ) == false )
        {
            return false;
        }

        const ThreadTaskPrefetchPtr & prefetcher = receiver->getPrefetcher();

        ThreadTaskPrefetchSoundDecoderPtr prefetcherSoundDecoder = ThreadTaskPrefetchSoundDecoderPtr::from( prefetcher );

        const SoundDecoderInterfacePtr & prefetch_decoder = prefetcherSoundDecoder->getDecoder();

        if( prefetch_decoder == nullptr )
        {
            return false;
        }

        *_decoder = prefetch_decoder;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::prefetchData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DataflowInterfacePtr & _dataflow, const DataflowContext * _context, const PrefetcherObserverInterfacePtr & _observer )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _dataflow, "file '%s' invalid dataflow is nullptr"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        if( this->isAvailableService() == false )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        if( m_threadQueue == nullptr )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        PrefetchReceiverPtr receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == true )
        {
            receiver->acquire();

            _observer->onPrefetchAlreadyExist();

            return true;
        }

        ThreadTaskPrefetchDataflowPtr task = m_factoryThreadTaskPrefetchDataflow->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->initialize( _fileGroup, _filePath, _observer );

        task->setDataflow( _dataflow );
        task->setDataflowContext( *_context );

        PrefetchReceiverPtr new_receiver = m_factoryPrefetchReceiver->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( new_receiver->initialize( task ) == false )
        {
            return false;
        }

        const ConstString & fileGroupName = _fileGroup->getName();

        m_prefetchReceivers.emplace( fileGroupName, _filePath, new_receiver );

        m_threadQueue->addTask( task );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::getData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, DataInterfacePtr * const _data )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        PrefetchReceiverPtr receiver;
        if( this->getPrefetch_( _fileGroup, _filePath, &receiver ) == false )
        {
            return false;
        }

        const ThreadTaskPrefetchPtr & prefetcher = receiver->getPrefetcher();

        ThreadTaskPrefetchDataflowPtr prefetcherDataflow = ThreadTaskPrefetchDataflowPtr::from( prefetcher );

        const DataInterfacePtr & prefetch_data = prefetcherDataflow->getData();

        if( prefetch_data == nullptr )
        {
            return false;
        }

        *_data = prefetch_data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::popData( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, DataInterfacePtr * const _data )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        ThreadTaskPrefetchPtr prefetcher;
        if( this->popPrefetch_( _fileGroup, _filePath, &prefetcher ) == false )
        {
            return false;
        }

        ThreadTaskPrefetchDataflowPtr prefetcherDataflow = ThreadTaskPrefetchDataflowPtr::from( prefetcher );

        const DataInterfacePtr & prefetch_data = prefetcherDataflow->getData();

        if( prefetch_data == nullptr )
        {
            return false;
        }

        *_data = prefetch_data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::prefetchStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ArchivatorInterfacePtr & _archivator, uint32_t _magicNumber, uint32_t _magicVersion, const PrefetcherObserverInterfacePtr & _observer )
    {
        if( this->isAvailableService() == false )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        if( m_threadQueue == nullptr )
        {
            _observer->onPrefetchIgnored();

            return true;
        }

        PrefetchReceiverPtr receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == true )
        {
            receiver->acquire();

            _observer->onPrefetchAlreadyExist();

            return true;
        }

        ThreadTaskPrefetchStreamPtr task = m_factoryThreadTaskPrefetchStream->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( task );

        task->initialize( _fileGroup, _filePath, _observer );

        task->setArchivator( _archivator );
        task->setMagicNumber( _magicNumber );
        task->setMagicVersion( _magicVersion );

        PrefetchReceiverPtr new_receiver = m_factoryPrefetchReceiver->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( new_receiver->initialize( task ) == false )
        {
            return false;
        }

        const ConstString & fileGroupName = _fileGroup->getName();

        m_prefetchReceivers.emplace( fileGroupName, _filePath, new_receiver );

        m_threadQueue->addTask( task );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::getStream( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, MemoryInterfacePtr * const _memory )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        PrefetchReceiverPtr receiver;
        if( this->getPrefetch_( _fileGroup, _filePath, &receiver ) == false )
        {
            return false;
        }

        const ThreadTaskPrefetchPtr & prefetcher = receiver->getPrefetcher();

        ThreadTaskPrefetchStreamPtr prefetcherStream = ThreadTaskPrefetchStreamPtr::from( prefetcher );

        const MemoryInterfacePtr & prefetch_memory = prefetcherStream->getMemory();

        if( prefetch_memory == nullptr )
        {
            return false;
        }

        *_memory = prefetch_memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::unfetch( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        const ConstString & fileGroupName = _fileGroup->getName();

        const PrefetchReceiverPtr & receiver = m_prefetchReceivers.find( fileGroupName, _filePath );

        if( receiver == nullptr )
        {
            return false;
        }

        if( receiver->release() == true )
        {
            return true;
        }

        const ThreadTaskPrefetchPtr & prefetcher = receiver->getPrefetcher();

        if( prefetcher->cancel() == true )
        {
            prefetcher->join();
        }

        m_prefetchReceivers.erase( fileGroupName, _filePath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetcherService::visitPrefetches( const VisitorPtr & _visitor ) const
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        for( const HashtablePrefetchReceiver::value_type & value : m_prefetchReceivers )
        {
            const ThreadTaskPrefetchPtr & prefetcher = value.element->getPrefetcher();

            prefetcher->visit( _visitor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::hasPrefetch_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, PrefetchReceiverPtr * const _receiver ) const
    {
        const ConstString & fileGroupName = _fileGroup->getName();

        const PrefetchReceiverPtr & receiver = m_prefetchReceivers.find( fileGroupName, _filePath );

        if( receiver == nullptr )
        {
            return false;
        }

        *_receiver = receiver;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::getPrefetch_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, PrefetchReceiverPtr * const _receiver ) const
    {
        PrefetchReceiverPtr receiver;
        if( this->hasPrefetch_( _fileGroup, _filePath, &receiver ) == false )
        {
            return false;
        }

        const ThreadTaskPrefetchPtr & prefetcher = receiver->getPrefetcher();

        if( prefetcher->isComplete() == false )
        {
            return false;
        }

        if( prefetcher->isSuccessful() == false )
        {
            return false;
        }

        *_receiver = receiver;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetcherService::popPrefetch_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ThreadTaskPrefetchPtr * const _prefetch )
    {
        const ConstString & fileGroupName = _fileGroup->getName();

        const PrefetchReceiverPtr & receiver = m_prefetchReceivers.find( fileGroupName, _filePath );

        if( receiver == nullptr )
        {
            return false;
        }

        ThreadTaskPrefetchPtr prefetcher = receiver->getPrefetcher();

        if( prefetcher->isComplete() == false ||
            prefetcher->isSuccessful() == false )
        {
            receiver->finalize();

            if( prefetcher->cancel() == true )
            {
                prefetcher->join();
            }

            m_prefetchReceivers.erase( fileGroupName, _filePath );

            return false;
        }

        *_prefetch = prefetcher;

        receiver->release();

        m_prefetchReceivers.erase( fileGroupName, _filePath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}