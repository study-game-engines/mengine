#pragma once

#include "Interface/EventationInterface.h"
#include "Interface/InputHandlerInterface.h"

#include "Kernel/Params.h"
#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/WheelCode.h"
#include "Kernel/TouchCode.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EGameEventFlags
    {
        EVENT_GAME_FULLSCREEN = 0,
        EVENT_GAME_FIXED_CONTENT_RESOLUTION,
        EVENT_GAME_FIXED_DISPLAY_RESOLUTION,
        EVENT_GAME_RENDER_VIEWPORT,
        EVENT_GAME_VIEWPORT,
        EVENT_GAME_KEY,
        EVENT_GAME_TEXT,
        EVENT_GAME_MOUSE_BUTTON,
        EVENT_GAME_MOUSE_BUTTON_BEGIN,
        EVENT_GAME_MOUSE_BUTTON_END,
        EVENT_GAME_MOUSE_MOVE,
        EVENT_GAME_MOUSE_WHEEL,
        EVENT_GAME_APP_MOUSE_ENTER,
        EVENT_GAME_APP_MOUSE_LEAVE,
        EVENT_GAME_ON_TIME_FACTOR,
        EVENT_GAME_PREPARATION,
        EVENT_GAME_RUN,
        EVENT_GAME_INTERRUPTION,
        EVENT_GAME_STOP,
        EVENT_GAME_INITIALIZE,
        EVENT_GAME_INITIALIZE_RENDER_RESOURCES,
        EVENT_GAME_FINALIZE_RENDER_RESOURCES,
        EVENT_GAME_ACCOUNT_FINALIZE,
        EVENT_GAME_FINALIZE,
        EVENT_GAME_DESTROY,
        EVENT_GAME_FOCUS,
        EVENT_GAME_CREATE_DEFAULT_ACCOUNT,
        EVENT_GAME_CREATE_GLOBAL_ACCOUNT,
        EVENT_GAME_LOAD_ACCOUNTS,
        EVENT_GAME_CREATE_ACCOUNT,
        EVENT_GAME_DELETE_ACCOUNT,
        EVENT_GAME_SELECT_ACCOUNT,
        EVENT_GAME_UNSELECT_ACCOUNT,
        EVENT_GAME_TURN_SOUND,
        EVENT_GAME_CHANGE_SOUND_VOLUME,
        EVENT_GAME_CURSOR_MODE,
        EVENT_GAME_USER,
        EVENT_GAME_CLOSE,
        EVENT_GAME_OVER_FILLRATE,
        EVENT_GAME_FRAME_END,
#if defined(MENGINE_PLATFORM_IOS)
        EVENT_GAME_IOS_APPLICATION_DID_BECOME_ACTIVE,
        EVENT_GAME_IOS_APPLICATION_WILL_ENTER_FOREGROUND,
        EVENT_GAME_IOS_APPLICATION_DID_ENTER_BACKGROUD,
        EVENT_GAME_IOS_APPLICATION_WILL_RESIGN_ACTIVE,
        EVENT_GAME_IOS_APPLICATION_WILL_TERMINATE,
#endif

        __EVENT_GAME_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class GameEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onGameFullscreen( bool _fullscreen ) = 0;
        virtual void onGameFixedContentResolution( bool _fixed ) = 0;
        virtual void onGameFixedDisplayResolution( bool _fixed ) = 0;
        virtual void onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) = 0;
        virtual void onGameViewport( const Viewport & _viewport, float _aspect ) = 0;
        virtual bool onGameKey( const InputKeyEvent & _event ) = 0;
        virtual bool onGameText( const InputTextEvent & _event ) = 0;
        virtual bool onGameMouseButton( const InputMouseButtonEvent & _event ) = 0;
        virtual bool onGameMouseButtonBegin( const InputMouseButtonEvent & _event ) = 0;
        virtual bool onGameMouseButtonEnd( const InputMouseButtonEvent & _event ) = 0;
        virtual bool onGameMouseMove( const InputMouseMoveEvent & _event ) = 0;
        virtual bool onGameMouseWheel( const InputMouseWheelEvent & _event ) = 0;
        virtual void onGameAppMouseEnter( const InputMouseEnterEvent & _event ) = 0;
        virtual void onGameAppMouseLeave( const InputMouseLeaveEvent & _event ) = 0;
        virtual void onGameTimeFactor( float _timeFactor ) = 0;
        virtual bool onGamePreparation( bool _debug ) = 0;
        virtual void onGameRun() = 0;
        virtual void onGameInterruption() = 0;
        virtual void onGameStop() = 0;
        virtual bool onGameInitialize() = 0;
        virtual void onGameInitializeRenderResources() = 0;
        virtual void onGameFinalizeRenderResources() = 0;
        virtual void onGameAccountFinalize() = 0;
        virtual void onGameFinalize() = 0;
        virtual void onGameDestroy() = 0;
        virtual void onGameFocus( bool _focus ) = 0;
        virtual void onGameCreateDefaultAccount() = 0;
        virtual void onGameCreateGlobalAccount() = 0;
        virtual void onGameLoadAccounts() = 0;
        virtual void onGameCreateAccount( const ConstString & _accountId, bool _global ) = 0;
        virtual void onGameDeleteAccount( const ConstString & _accountId ) = 0;
        virtual void onGameSelectAccount( const ConstString & _accountId ) = 0;
        virtual void onGameUselectAccount( const ConstString & _accountId ) = 0;
        virtual void onGameTurnSound( bool _turn ) = 0;
        virtual void onGameChangeSoundVolume( float _sound, float _music, float _voice ) = 0;
        virtual void onGameCursorMode( bool _mode ) = 0;
        virtual void onGameUser( const ConstString & _event, const MapWParams & _params ) = 0;
        virtual bool onGameClose() = 0;
        virtual void onGameOverFillrate( double _fillrate, double _limit ) = 0;
        virtual void onGameFrameEnd() = 0;
#if defined(MENGINE_PLATFORM_IOS)
        virtual void onGameiOSApplicationDidBecomeActive() = 0;
        virtual void onGameiOSApplicationWillEnterForeground() = 0;
        virtual void onGameiOSApplicationDidEnterBackground() = 0;
        virtual void onGameiOSApplicationWillResignActive() = 0;
        virtual void onGameiOSApplicationWillTerminate() = 0;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FULLSCREEN );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FIXED_CONTENT_RESOLUTION );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FIXED_DISPLAY_RESOLUTION );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_RENDER_VIEWPORT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_VIEWPORT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_KEY );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_TEXT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_BUTTON );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_BUTTON_BEGIN );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_BUTTON_END );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_MOVE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_WHEEL );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_APP_MOUSE_ENTER );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_APP_MOUSE_LEAVE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_ON_TIME_FACTOR );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_PREPARATION );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_RUN );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_INTERRUPTION );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_STOP );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_INITIALIZE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_INITIALIZE_RENDER_RESOURCES );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_ACCOUNT_FINALIZE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FINALIZE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FINALIZE_RENDER_RESOURCES );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_DESTROY );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FOCUS );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CREATE_DEFAULT_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CREATE_GLOBAL_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_LOAD_ACCOUNTS );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CREATE_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_DELETE_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_SELECT_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_TURN_SOUND );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_UNSELECT_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CHANGE_SOUND_VOLUME );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CURSOR_MODE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_USER );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CLOSE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_OVER_FILLRATE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FRAME_END );
#if defined(MENGINE_PLATFORM_IOS)
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_IOS_APPLICATION_DID_BECOME_ACTIVE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_IOS_APPLICATION_WILL_ENTER_FOREGROUND );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_IOS_APPLICATION_DID_ENTER_BACKGROUD );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_IOS_APPLICATION_WILL_RESIGN_ACTIVE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_IOS_APPLICATION_WILL_TERMINATE );
#endif
    //////////////////////////////////////////////////////////////////////////
}