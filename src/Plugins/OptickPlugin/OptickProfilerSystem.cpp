#include "OptickProfilerSystem.h"

#include "Interface/PlatformInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "OptickThreadProfiler.h"
#include "OptickFrameProfiler.h"
#include "OptickCategoryProfiler.h"
#include "OptickProfilerDescription.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/FilePathDateTimeHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ThreadMutexScope.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ProfilerSystem, Mengine::OptickProfilerSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OptickProfilerSystem::OptickProfilerSystem()
        : m_process( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OptickProfilerSystem::~OptickProfilerSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptickProfilerSystem::_initializeService()
    {
        m_factoryThreadProfilers = Helper::makeFactoryPool<OptickThreadProfiler, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryFrameProfilers = Helper::makeFactoryPool<OptickFrameProfiler, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryCategoryProfilers = Helper::makeFactoryPool<OptickCategoryProfiler, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryDescriptions = Helper::makeFactoryPool<OptickProfilerDescription, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );

        SERVICE_WAIT( ThreadServiceInterface, [this]()
        {
            ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
                ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

            m_factoryThreadProfilers->setMutex( mutex );
            m_factoryFrameProfilers->setMutex( mutex );
            m_factoryCategoryProfilers->setMutex( mutex );
            m_factoryDescriptions->setMutex( mutex );

            m_mutex = mutex;

            return true;
        } );

        SERVICE_LEAVE( ThreadServiceInterface, [this]()
        {
            m_factoryThreadProfilers->setMutex( nullptr );
            m_factoryFrameProfilers->setMutex( nullptr );
            m_factoryCategoryProfilers->setMutex( nullptr );
            m_factoryDescriptions->setMutex( nullptr );

            m_mutex = nullptr;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OptickProfilerSystem::_finalizeService()
    {
        m_descriptions.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadProfilers );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryFrameProfilers );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryCategoryProfilers );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDescriptions );

        m_factoryThreadProfilers = nullptr;
        m_factoryFrameProfilers = nullptr;
        m_factoryCategoryProfilers = nullptr;
        m_factoryDescriptions = nullptr;        
    }
    //////////////////////////////////////////////////////////////////////////
    void OptickProfilerSystem::beginApplication()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        Optick::RegisterThread( "main" );
        Optick::StartCapture();

        m_process = true;
    }
    //////////////////////////////////////////////////////////////////////////
    static OutputStreamInterface * g_outputFile = nullptr;
    //////////////////////////////////////////////////////////////////////////
    void OptickProfilerSystem::endApplication()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_process = false;

        Optick::UnRegisterThread( false );
        Optick::StopCapture();

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        Char userPath[MENGINE_MAX_PATH] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserPath( userPath );

        String optPath;
        optPath += "Optick";
        optPath += "_";

        DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
            ->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        Char filePathDate[MENGINE_MAX_PATH] = {'\0'};
        Helper::makeFilePathDateTimeHelper( dateTimeProvider, filePathDate );

        optPath += filePathDate;
        optPath += ".opt";

        FilePath cs_optPath = Helper::stringizeFilePath( optPath );

        OutputStreamInterfacePtr outputFile = fileGroup->createOutputFile( MENGINE_DOCUMENT_FACTORABLE );

        fileGroup->openOutputFile( cs_optPath, outputFile, false );

        //ToDo FixMe [https://github.com/bombomby/optick/issues/137]
        g_outputFile = outputFile.get();

        Optick::SaveCapture( []( const char * _data, size_t _size )
        {
            g_outputFile->write( _data, _size );
        }, true );
    }
    //////////////////////////////////////////////////////////////////////////
    ProfilerInterfacePtr OptickProfilerSystem::addThread( const Char * _threadName )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        OptickThreadProfilerPtr profiler = m_factoryThreadProfilers->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( profiler->initialize( _threadName ) == false )
        {
            return nullptr;
        }

        return profiler;
    }
    //////////////////////////////////////////////////////////////////////////
    ProfilerInterfacePtr OptickProfilerSystem::addFrame( ProfilerDescriptionInterface * _description )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( m_process == false )
        {
            return nullptr;
        }

        OptickFrameProfilerPtr profiler = m_factoryFrameProfilers->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( profiler->initialize( _description ) == false )
        {
            return nullptr;
        }

        return profiler;
    }
    //////////////////////////////////////////////////////////////////////////
    ProfilerInterfacePtr OptickProfilerSystem::addCategory( ProfilerDescriptionInterface * _description )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( m_process == false )
        {
            return nullptr;
        }

        OptickCategoryProfilerPtr profiler = m_factoryCategoryProfilers->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( profiler->initialize( _description ) == false )
        {
            return nullptr;
        }

        return profiler;
    }
    //////////////////////////////////////////////////////////////////////////
    ProfilerDescriptionInterface * OptickProfilerSystem::createDescription( const Char * _name, const Char * _file, uint32_t _line )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( m_process == false )
        {
            return nullptr;
        }

        OptickProfilerDescriptionPtr description = m_factoryDescriptions->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( description->initialize( _name, _file, _line ) == false )
        {
            return nullptr;
        }

        m_descriptions.emplace_back( description );

        ProfilerDescriptionInterface * description_ptr = description.get();

        return description_ptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
