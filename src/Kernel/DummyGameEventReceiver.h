#pragma once

#include "Interface/GameEventReceiverInterface.h"

namespace Mengine
{
    class DummyGameEventReceiver
        : public GameEventReceiverInterface
    {
    public:
        DummyGameEventReceiver();
        ~DummyGameEventReceiver() override;

    public:
        void onGameFullscreen( bool _fullscreen ) override;
        void onGameFixedContentResolution( bool _fixed ) override;
        void onGameFixedDisplayResolution( bool _fixed ) override;
        void onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) override;
        void onGameViewport( const Viewport & _viewport, float _aspect ) override;
        bool onGameKey( const InputKeyEvent & _event ) override;
        bool onGameText( const InputTextEvent & _event ) override;
        bool onGameMouseButton( const InputMouseButtonEvent & _event ) override;
        bool onGameMouseButtonBegin( const InputMouseButtonEvent & _event ) override;
        bool onGameMouseButtonEnd( const InputMouseButtonEvent & _event ) override;
        bool onGameMouseMove( const InputMouseMoveEvent & _event ) override;
        bool onGameMouseWheel( const InputMouseWheelEvent & _event ) override;
        void onGameAppMouseEnter( const InputMouseEnterEvent & _event ) override;
        void onGameAppMouseLeave( const InputMouseLeaveEvent & _event ) override;
        void onGameTimeFactor( float _timeFactor ) override;
        bool onGamePreparation( bool _debug ) override;
        void onGameRun() override;
        void onGameStop() override;
        bool onGameInitialize() override;
        void onGameInitializeRenderResources() override;
        void onGameFinalizeRenderResources() override;
        void onGameAccountFinalize() override;
        void onGameFinalize() override;
        void onGameDestroy() override;
        void onGameFocus( bool _focus ) override;
        void onGameCreateDefaultAccount() override;
        void onGameCreateGlobalAccount() override;
        void onGameLoadAccounts() override;
        void onGameCreateAccount( const ConstString & _accountID, bool _global ) override;
        void onGameDeleteAccount( const ConstString & _accountID ) override;
        void onGameSelectAccount( const ConstString & _accountID ) override;
        void onGameUselectAccount( const ConstString & _accountID ) override;
        void onGameTurnSound( bool _turn ) override;
        void onGameChangeSoundVolume( float _sound, float _music, float _voice ) override;
        void onGameCursorMode( bool _mode ) override;
        void onGameUser( const ConstString & _event, const MapWParams & _params ) override;
        bool onGameClose() override;
        void onGameOverFillrate( double _fillrate, double _limit ) override;
        void onGameFrameEnd() override;
#if defined(MENGINE_PLATFORM_IOS)
        void onGameiOSApplicationDidBecomeActive() override;
        void onGameiOSApplicationWillEnterForeground() override;
        void onGameiOSApplicationDidEnterBackground() override;
        void onGameiOSApplicationWillResignActive() override;
        void onGameiOSApplicationWillTerminate() override;
#endif
    };
}