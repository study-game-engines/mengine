#include "PlayerService.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/GraveyardServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"

#include "Plugins/AstralaxPlugin/AstralaxInterface.h"
#include "Plugins/NodeDebugRenderPlugin/NodeDebugRenderServiceInterface.h"

#include "PlayerGlobalInputHandler.h"
#include "PlayerGlobalAffectorable.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Scene.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraHelper.h"
#include "Kernel/MT19937Randomizer.h"
#include "Kernel/Arrow.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Scheduler.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/DocumentableHelper.h"
#include "Kernel/Resource.h"
#include "Kernel/Documentable.h"
#include "Kernel/RenderContextHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/TimeHelper.h"

#include "Config/Algorithm.h"

#include "math/mat3.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlayerService, Mengine::PlayerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PlayerService::PlayerService()
        : m_focus( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PlayerService::~PlayerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::_initializeService()
    {
        m_globalInputHandler = Helper::makeFactorableUnique<PlayerGlobalInputHandler>( MENGINE_DOCUMENT_FACTORABLE );

        m_factoryScheduleManager = Helper::makeFactoryPool<Scheduler, 16>( MENGINE_DOCUMENT_FACTORABLE );

        SchedulerPtr scheduler = m_factoryScheduleManager->createObject( MENGINE_DOCUMENT_FACTORABLE );

        scheduler->setName( STRINGIZE_STRING_LOCAL( "LocalScheduleManager" ) );

        if( scheduler->initialize() == false )
        {
            return false;
        }

        m_localScheduler = scheduler;

        SchedulerPtr schedulerGlobal = m_factoryScheduleManager->createObject( MENGINE_DOCUMENT_FACTORABLE );

        schedulerGlobal->setName( STRINGIZE_STRING_LOCAL( "GlobalScheduleManager" ) );

        if( schedulerGlobal->initialize() == false )
        {
            return false;
        }

        m_globalScheduler = schedulerGlobal;

        m_randomizer = Helper::makeFactorableUnique<MT19937Randomizer>( MENGINE_DOCUMENT_FACTORABLE );

        if( HAS_OPTION( "seed" ) == true )
        {
            uint32_t randomSeed = GET_OPTION_VALUE_UINT32( "seed", 0U );

            m_randomizer->setSeed( randomSeed );
        }
        else
        {
            TimeMilliseconds milliseconds = Helper::getTimeMilliseconds();

            uint32_t randomSeed = (uint32_t)milliseconds;

            m_randomizer->setSeed( randomSeed );
        }

        m_affectorable = Helper::makeFactorableUnique<PlayerGlobalAffectorable>( MENGINE_DOCUMENT_FACTORABLE );
        m_affectorableGlobal = Helper::makeFactorableUnique<PlayerGlobalAffectorable>( MENGINE_DOCUMENT_FACTORABLE );

        Helper::clearRenderContext( &m_renderContext );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY, &PlayerService::notifyChangeScenePrepareDestroy, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_DESTROY, &PlayerService::notifyChangeSceneDestroy, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_INITIALIZE, &PlayerService::notifyChangeScenePrepareInitialize, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_ENABLE, &PlayerService::notifyChangeSceneEnable, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_PREPARE_COMPLETE, &PlayerService::notifyChangeScenePrepareComplete, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RESTART_SCENE_PREPARE_DISABLE, &PlayerService::notifyRestartScenePrepareDisable, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RESTART_SCENE_DISABLE, &PlayerService::notifyRestartSceneDisable, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RESTART_SCENE_ENABLE, &PlayerService::notifyRestartSceneEnable, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_REMOVE_SCENE_PREPARE_DESTROY, &PlayerService::notifyRemoveScenePrepareDestroy, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_REMOVE_SCENE_DESTROY, &PlayerService::notifyRemoveSceneDestroy, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_DESTROY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_INITIALIZE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_ENABLE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_PREPARE_COMPLETE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RESTART_SCENE_PREPARE_DISABLE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RESTART_SCENE_DISABLE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RESTART_SCENE_ENABLE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_REMOVE_SCENE_PREPARE_DESTROY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_REMOVE_SCENE_DESTROY );

        m_globalInputHandler = nullptr;

        m_localScheduler = nullptr;
        m_globalScheduler = nullptr;

        for( const SchedulerInterfacePtr & scheduler : m_schedulers )
        {
#if defined(MENGINE_DEBUG)
            const Char * doc = MENGINE_DOCUMENTABLE_STR( scheduler.get(), "forgotten scheduler" );

            LOGGER_ERROR( "was forgotten finalize scheduler '%s' (doc: %s)"
                , scheduler->getName().c_str()
                , doc
            );
#endif

            scheduler->finalize();
        }

        m_schedulers.clear();

        m_randomizer = nullptr;

        m_affectorable = nullptr;
        m_affectorableGlobal = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScheduleManager );

        m_factoryScheduleManager = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::_stopService()
    {
        if( m_arrow != nullptr )
        {
            m_arrow->disable();

            RenderInterface * arrowRender = m_arrow->getRender();

            arrowRender->setRenderCamera( nullptr );
            arrowRender->setRenderViewport( nullptr );
            arrowRender->setRenderTransformation( nullptr );
            arrowRender->setRenderScissor( nullptr );
            arrowRender->setRenderTarget( nullptr );

            PICKER_SERVICE()
                ->setArrow( nullptr );

            m_arrow->dispose();
            m_arrow = nullptr;
        }

        m_renderViewport = nullptr;
        m_renderCamera = nullptr;
        m_renderTransformation = nullptr;
        m_renderScissor = nullptr;
        m_renderTarget = nullptr;

        Helper::clearRenderContext( &m_renderContext );

        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->finalize();
        }

        if( m_localScheduler != nullptr )
        {
            m_localScheduler->finalize();
        }

        if( m_globalScheduler != nullptr )
        {
            m_globalScheduler->finalize();
        }

        if( m_localScheduler != nullptr )
        {
            m_localScheduler->removeAll();
        }

        if( m_globalScheduler != nullptr )
        {
            m_globalScheduler->removeAll();
        }

        if( m_affectorable != nullptr )
        {
            if( m_affectorable->availableAffectorHub() == true )
            {
                const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

                affectorHub->stopAllAffectors();
                m_affectorable->clearAffectorHub();
            }
        }

        if( m_affectorableGlobal != nullptr )
        {
            if( m_affectorableGlobal->availableAffectorHub() == true )
            {
                const AffectorHubInterfacePtr & affectorHub = m_affectorableGlobal->getAffectorHub();

                affectorHub->stopAllAffectors();
                m_affectorableGlobal->clearAffectorHub();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setArrow( const ArrowPtr & _arrow )
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_ARROW_PREPARE_DESTROY, m_arrow );

        if( m_arrow != nullptr )
        {
            m_arrow->disable();

            RenderInterface * arrowRender = m_arrow->getRender();

            arrowRender->setRenderCamera( nullptr );
            arrowRender->setRenderViewport( nullptr );
            arrowRender->setRenderTransformation( nullptr );
            arrowRender->setRenderScissor( nullptr );
            arrowRender->setRenderTarget( nullptr );

            PICKER_SERVICE()
                ->setArrow( nullptr );

            m_arrow->dispose();
            m_arrow = nullptr;
        }

        m_arrow = _arrow;

        if( m_arrow != nullptr )
        {
            RenderInterface * arrowRender = m_arrow->getRender();

            arrowRender->setRenderCamera( m_defaultArrowCamera2D );
            arrowRender->setRenderViewport( m_renderViewport );
            arrowRender->setRenderScissor( m_renderScissor );

            PICKER_SERVICE()
                ->setArrow( m_arrow );

            m_arrow->enable();
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_ARROW_COMPLETE, m_arrow );
    }
    //////////////////////////////////////////////////////////////////////////
    const ArrowPtr & PlayerService::getArrow() const
    {
        return m_arrow;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint )
    {
        Helper::screenToWorldPosition( &m_renderContext, _screenPoint, _worldPoint );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::calcGlobalMouseWorldDelta( const mt::vec2f & _screenDeltha, mt::vec2f * const _worldDeltha )
    {
        Helper::screenToWorldDelta( &m_renderContext, _screenDeltha, _worldDeltha );
    }
    //////////////////////////////////////////////////////////////////////////
    SchedulerInterfacePtr PlayerService::createScheduler( const ConstString & _name, const DocumentPtr & _doc )
    {
        SchedulerPtr sm = m_factoryScheduleManager->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( sm );

        sm->setName( _name );

        if( sm->initialize() == false )
        {
            return nullptr;
        }

        m_schedulers.emplace_back( sm );

        return sm;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::destroyScheduler( const SchedulerInterfacePtr & _scheduler )
    {
        VectorUserScheduler::iterator it_found = Algorithm::find( m_schedulers.begin(), m_schedulers.end(), _scheduler );

        if( it_found == m_schedulers.end() )
        {
            LOGGER_ERROR( "scheduler '%s' not found!"
                , _scheduler->getName().c_str()
            );

            return false;
        }

        _scheduler->finalize();

        m_schedulers.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const GlobalInputHandlerInterfacePtr & PlayerService::getGlobalInputHandler() const
    {
        return m_globalInputHandler;
    }
    //////////////////////////////////////////////////////////////////////////
    const SchedulerInterfacePtr & PlayerService::getLocalScheduler() const
    {
        return m_localScheduler;
    }
    //////////////////////////////////////////////////////////////////////////
    const SchedulerInterfacePtr & PlayerService::getGlobalScheduler() const
    {
        return m_globalScheduler;
    }
    //////////////////////////////////////////////////////////////////////////
    const RandomizerInterfacePtr & PlayerService::getRandomizer() const
    {
        return m_randomizer;
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorHubInterfacePtr & PlayerService::getAffectorHub() const
    {
        const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

        return affectorHub;
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorHubInterfacePtr & PlayerService::getGlobalAffectorHub() const
    {
        const AffectorHubInterfacePtr & affectorHub = m_affectorableGlobal->getAffectorHub();

        return affectorHub;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::initializeRenderResources()
    {
        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f cr;
        contentResolution.calcSize( &cr );
        Viewport vp( 0.f, 0.f, cr.x, cr.y );

        float gameViewportAspect;
        Viewport gameViewport;

        APPLICATION_SERVICE()
            ->getGameViewport( &gameViewportAspect, &gameViewport );

        m_defaultCamera2D = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderCameraOrthogonal" ), MENGINE_DOCUMENT_FACTORABLE );

        m_defaultCamera2D->setName( STRINGIZE_STRING_LOCAL( "DefaultCamera2D" ) );

        m_defaultCamera2D->setOrthogonalViewport( gameViewport );
        m_defaultCamera2D->enableForce();

        this->setRenderCamera( m_defaultCamera2D );

        m_defaultViewport2D = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderViewport" ), MENGINE_DOCUMENT_FACTORABLE );

        m_defaultViewport2D->setName( STRINGIZE_STRING_LOCAL( "DefaultViewport2D" ) );

        m_defaultViewport2D->setViewport( vp );
        m_defaultViewport2D->enableForce();

        this->setRenderViewport( m_defaultViewport2D );

        m_defaultArrowCamera2D = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderCameraOrthogonal" ), MENGINE_DOCUMENT_FACTORABLE );

        m_defaultArrowCamera2D->setName( STRINGIZE_STRING_LOCAL( "DefaultArrowCamera2D" ) );

        m_defaultArrowCamera2D->setOrthogonalViewport( gameViewport );
        m_defaultArrowCamera2D->enableForce();

        if( m_arrow != nullptr )
        {
            RenderInterface * arrowRender = m_arrow->getRender();

            arrowRender->setRenderCamera( m_defaultArrowCamera2D );
            arrowRender->setRenderViewport( m_renderViewport );
            arrowRender->setRenderScissor( m_renderScissor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::finalizeRenderResources()
    {
        this->setRenderCamera( nullptr );
        this->setRenderViewport( nullptr );
        this->setRenderScissor( nullptr );

        if( m_defaultCamera2D != nullptr )
        {
            m_defaultCamera2D->disable();
            m_defaultCamera2D = nullptr;
        }

        if( m_defaultViewport2D != nullptr )
        {
            m_defaultViewport2D->disable();
            m_defaultViewport2D = nullptr;
        }

        if( m_defaultArrowCamera2D != nullptr )
        {
            m_defaultArrowCamera2D->disable();
            m_defaultArrowCamera2D = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleKeyEvent( const InputKeyEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleKeyEvent( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleKeyEvent( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleTextEvent( const InputTextEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleTextEvent( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleTextEvent( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseButtonEvent( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseButtonEvent( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseButtonEventBegin( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseButtonEventBegin( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseButtonEventEnd( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseButtonEventEnd( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseMove( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseMove( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseWheel( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseWheel( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::update()
    {
        PICKER_SERVICE()
            ->update();

        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->update();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraOrthogonalPtr & PlayerService::getDefaultSceneRenderCamera2D() const
    {
        return m_defaultCamera2D;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportPtr & PlayerService::getDefaultRenderViewport2D() const
    {
        return m_defaultViewport2D;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraOrthogonalPtr & PlayerService::getDefaultArrowRenderCamera2D() const
    {
        return m_defaultArrowCamera2D;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setRenderViewport( const RenderViewportInterfacePtr & _viewport )
    {
        m_renderViewport = _viewport;

        m_renderContext.viewport = m_renderViewport.get();

        PICKER_SERVICE()
            ->setRenderViewport( m_renderViewport );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & PlayerService::getRenderViewport() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setRenderCamera( const RenderCameraInterfacePtr & _camera )
    {
        m_renderCamera = _camera;

        m_renderContext.camera = m_renderCamera.get();

        PICKER_SERVICE()
            ->setRenderCamera( m_renderCamera );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & PlayerService::getRenderCamera() const
    {
        return m_renderCamera;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setRenderTransformation( const RenderTransformationInterfacePtr & _transformation )
    {
        m_renderTransformation = _transformation;

        m_renderContext.transformation = m_renderTransformation.get();

        PICKER_SERVICE()
            ->setRenderTransformation( m_renderTransformation );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTransformationInterfacePtr & PlayerService::getRenderTransformation() const
    {
        return m_renderTransformation;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setRenderScissor( const RenderScissorInterfacePtr & _scissor )
    {
        m_renderScissor = _scissor;

        PICKER_SERVICE()
            ->setRenderScissor( _scissor );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderScissorInterfacePtr & PlayerService::getRenderScissor() const
    {
        return m_renderScissor;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setRenderTarget( const RenderTargetInterfacePtr & _target )
    {
        m_renderTarget = _target;

        m_renderContext.target = m_renderTarget.get();

        PICKER_SERVICE()
            ->setRenderTarget( m_renderTarget );
    }
    const RenderTargetInterfacePtr & PlayerService::getRenderTarget() const
    {
        return m_renderTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderContext * PlayerService::getRenderContext() const
    {
        return &m_renderContext;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::render( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        MENGINE_PROFILER_CATEGORY();

        uint32_t debugMask = APPLICATION_SERVICE()
            ->getDebugMask();

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene != nullptr )
        {
            if( debugMask == 0 )
            {
                Helper::nodeRenderChildren( scene.get(), _renderPipeline, &m_renderContext, false );
            }
            else
            {
                if( SERVICE_IS_INITIALIZE( NodeDebugRenderServiceInterface ) == true )
                {
                    NODEDEBUGRENDER_SERVICE()
                        ->renderDebugNode( scene, _renderPipeline, &m_renderContext, false, false );
                }
            }
        }

        MODULE_SERVICE()
            ->render( _renderPipeline, &m_renderContext );

        _renderPipeline->endDebugLimitRenderObjects();

        if( m_arrow != nullptr )
        {
            if( debugMask == 0 )
            {
                Helper::nodeRenderChildren( m_arrow.get(), _renderPipeline, &m_renderContext, false );
            }
            else
            {
                if( SERVICE_IS_INITIALIZE( NodeDebugRenderServiceInterface ) == true )
                {
                    NODEDEBUGRENDER_SERVICE()
                        ->renderDebugNode( m_arrow, _renderPipeline, &m_renderContext, false, false );
                }
            }
        }

        if( SERVICE_IS_INITIALIZE( NodeDebugRenderServiceInterface ) == true )
        {
            NODEDEBUGRENDER_SERVICE()
                ->renderDebugInfo( _renderPipeline, &m_renderContext );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseEnter( _event );
        }

        if( m_arrow != nullptr )
        {
            m_arrow->onAppMouseEnter();
        }

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene != nullptr && scene->isActivate() == true )
        {
            scene->onAppMouseEnter();
        }

        PICKER_SERVICE()
            ->handleMouseEnter( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseLeave( _event );
        }

        if( m_arrow != nullptr )
        {
            m_arrow->onAppMouseLeave();
        }

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene != nullptr && scene->isActivate() == true )
        {
            scene->onAppMouseLeave();
        }

        PICKER_SERVICE()
            ->handleMouseLeave( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onFullscreen( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );
        MENGINE_UNUSED( _fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
        MENGINE_UNUSED( _resolution );
        MENGINE_UNUSED( _fixed );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onFixedDisplayResolution( const Resolution & _resolution, bool _fixed )
    {
        MENGINE_UNUSED( _resolution );
        MENGINE_UNUSED( _fixed );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onFocus( bool _focus )
    {
        if( m_focus == _focus )
        {
            return;
        }

        m_focus = _focus;

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene != nullptr && scene->isActivate() == true )
        {
            scene->onFocus( m_focus );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeScenePrepareDestroy( const ScenePtr & _oldScene, const ScenePtr & _newScene )
    {
        MENGINE_UNUSED( _oldScene );
        MENGINE_UNUSED( _newScene );

        if( m_arrow != nullptr )
        {
            m_arrow->removeFromParent();
            m_arrow->disable();
        }

        if( m_localScheduler != nullptr )
        {
            m_localScheduler->removeAll();
        }

        if( m_affectorable != nullptr )
        {
            if( m_affectorable->availableAffectorHub() == true )
            {
                const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

                affectorHub->stopAllAffectors();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeSceneDestroy( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( SERVICE_IS_INITIALIZE( GraveyardServiceInterface ) == true )
        {
            GRAVEYARD_SERVICE()
                ->clearTextures();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeScenePrepareInitialize( const ScenePtr & _scene )
    {
        PICKER_SERVICE()
            ->setScene( _scene );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeSceneEnable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( m_arrow != nullptr )
        {
            m_arrow->enableForce();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeScenePrepareComplete( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        MEMORY_SERVICE()
            ->clearCacheBuffers();
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRestartScenePrepareDisable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( m_arrow != nullptr )
        {
            m_arrow->removeFromParent();
            m_arrow->disable();
        }

        if( m_localScheduler != nullptr )
        {
            m_localScheduler->removeAll();
        }

        if( m_affectorable != nullptr )
        {
            if( m_affectorable->availableAffectorHub() == true )
            {
                const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

                affectorHub->stopAllAffectors();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRestartSceneDisable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( SERVICE_IS_INITIALIZE( GraveyardServiceInterface ) == true )
        {
            GRAVEYARD_SERVICE()
                ->clearTextures();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRestartSceneEnable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( m_arrow != nullptr )
        {
            m_arrow->enableForce();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRemoveScenePrepareDestroy( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( m_arrow != nullptr )
        {
            m_arrow->removeFromParent();
            m_arrow->disable();
        }

        if( m_localScheduler != nullptr )
        {
            m_localScheduler->removeAll();
        }

        if( m_affectorable != nullptr )
        {
            if( m_affectorable->availableAffectorHub() == true )
            {
                const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

                affectorHub->stopAllAffectors();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRemoveSceneDestroy()
    {
        PICKER_SERVICE()
            ->setScene( nullptr );

        if( SERVICE_IS_INITIALIZE( GraveyardServiceInterface ) == true )
        {
            GRAVEYARD_SERVICE()
                ->clearTextures();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
