#pragma once

#include "Interface/ApplicationInterface.h"

#include "Interface/StringizeInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/UnicodeInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ScriptSystemInterface.h"
#include "Interface/ParticleSystemInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/InputSystemInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/ArchiveInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ConfigInterface.h"
#include "Interface/PrefetcherInterface.h"

#include "Kernel/FileLogger.h"
#include "Kernel/ServiceBase.h"

#include "SDLInput.h"

#include "SDL.h"

namespace Mengine
{
    class SDLPlatform
        : public ServiceBase<PlatformInterface>
    {
    public:
        SDLPlatform();
        ~SDLPlatform();

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void update() override;
        void stopPlatform()	override;

    public:
        void setIcon( uint32_t _icon ) override;
        uint32_t getIcon() const override;

        void setProjectTitle( const WString & _projectTitle ) override;
        const WString & getProjectTitle() const override;

    public:
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;
        Pointer getWindowHandle() const override;

    public:
        bool hasPlatformTag( const ConstString & _tag ) const override;
        const Tags & getPlatformTags() const override;

    public:
        bool hasTouchpad() const override;

    public:
        bool getDesktopResolution( Resolution & _resolution ) const override;

		bool getNoFullscreen() const override;
		bool getAlwaysFullscreen() const override;

        size_t getCurrentPath( WChar * _path, size_t _len ) const override;
		size_t getUserPath( WChar * _path, size_t _len ) const override;

        void minimizeWindow() override;

        void setCursorPosition( const mt::vec2f & _pos ) override;

        void showKeyboard() override;
        void hideKeyboard() override;

        void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyVsyncChanged( bool _vsync ) override;
        void notifyCursorModeChanged( bool _mode ) override;
        bool notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer ) override;

    public:
        void onEvent( const ConstString & _event, const MapWParams & _params ) override;

    public:
        float getJoystickAxis( uint32_t _index ) const override;

    public:
        size_t getShortPathName( const WString & _path, WChar * _short, size_t _len ) const override;

    public:
        void getMaxClientResolution( Resolution & _resolution ) const override;

    public:
        bool openUrlInDefaultBrowser( const WString & _url ) override;
        
    public:
		bool existDirectory( const WString & _path ) const override;
		bool createDirectory(const WString & _path) override;

	public:
		bool removeFile( const WString & _path ) override;

	public:
		uint64_t getFileTime( const WString & _filePath ) const override;

	public:
        bool createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;
        
    protected:
        void changeWindow_( const Resolution & _resolution, bool _fullscreen );
        bool createWindow_( const Resolution & _resolution, bool _fullscreen );
        void destroyWindow_();

    protected:
        bool processEvents();

    protected:
        void setActive_( bool _active );

    protected:
        Tags m_platformName;

        SDL_Window * m_window;

        SDL_Joystick * m_accelerometer;
        
        SDL_GLContext m_glContext;

        SDLInputPtr m_sdlInput;

        uint32_t m_icon;
        WString m_projectTitle;
        
        bool m_shouldQuit;
        bool m_running;
        bool m_pause;

        bool m_touchpad;
    };
}
