#include "SilentSoundSystem.h"

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SilentSoundSystem, Mengine::SilentSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SilentSoundSystem::SilentSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SilentSoundSystem::~SilentSoundSystem()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSystem::_initializeService()
    {
        LOGGER_INFO( "sound", "Starting Silent Sound System..." );

        m_factorySilentSoundBuffer = Helper::makeFactoryPool<SilentSoundBuffer, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factorySilentSoundSource = Helper::makeFactoryPool<SilentSoundSource, 32>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySilentSoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySilentSoundSource );

        m_factorySilentSoundBuffer = nullptr;
        m_factorySilentSoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSystem::isSilent() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::onTurnSound( bool _turn )
    {
        MENGINE_UNUSED( _turn );
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr SilentSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentPtr & _doc )
    {
        SilentSoundSourcePtr soundSource = m_factorySilentSoundSource->createObject( _doc );

        soundSource->initialize( this );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr SilentSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable, const DocumentPtr & _doc )
    {
        SilentSoundBufferPtr buffer = m_factorySilentSoundBuffer->createObject( _doc );

        if( buffer->load( _soundDecoder, _streamable ) == false )
        {
            LOGGER_ERROR( "failed to load sound buffer from decoder" );

            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::genSourceId()
    {
        UniqueId new_id = GENERATE_UNIQUE_IDENTITY();

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::releaseSourceId( uint32_t _sourceId )
    {
        MENGINE_UNUSED( _sourceId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::genBufferId()
    {
        UniqueId new_id = GENERATE_UNIQUE_IDENTITY();

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::releaseBufferId( uint32_t _bufferId )
    {
        MENGINE_UNUSED( _bufferId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::clearSourceId( uint32_t _sourceId )
    {
        MENGINE_UNUSED( _sourceId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::getSourcesCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::getBuffersCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
