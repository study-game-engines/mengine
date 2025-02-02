#include "Win32CriticalErrorsMonitorPlugin.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"

#include "Interface/Win32PlatformServiceExtensionInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32CriticalErrorsMonitorPlugin, Mengine::Win32CriticalErrorsMonitorPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32CriticalErrorsMonitorPlugin::Win32CriticalErrorsMonitorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32CriticalErrorsMonitorPlugin::~Win32CriticalErrorsMonitorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CriticalErrorsMonitorPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "criticalerrorsmonitor" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nocriticalerrorsmonitor" ) == true )
        {
            return false;
        }

        bool Win32CriticalErrorsMonitorPlugin_Available = CONFIG_VALUE( "Win32CriticalErrorsMonitorPlugin", "Available", true );

        if( Win32CriticalErrorsMonitorPlugin_Available == false )
        {
            return false;
        }

        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static Win32CriticalErrorsMonitorPlugin * g_monitor = nullptr;
    //////////////////////////////////////////////////////////////////////////
    bool Win32CriticalErrorsMonitorPlugin::s_writeCrashDump( EXCEPTION_POINTERS * pExceptionPointers )
    {
        if( g_monitor == nullptr )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
            ->createProcessDump( g_monitor->m_dumpPath.c_str(), pExceptionPointers, false ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    LONG WINAPI Win32CriticalErrorsMonitorPlugin::s_exceptionHandler( EXCEPTION_POINTERS * pExceptionPointers )
    {
        MENGINE_UNUSED( pExceptionPointers );

        LOGGER_ERROR( "exception catch" );

#if defined(MENGINE_PLATFORM_WINDOWS)
        Win32PlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
            ->getUnknown();

        Char stack[8096] = {'\0'};
        if( extension->getCallstack( ~0U, stack, 8095, pExceptionPointers->ContextRecord ) == false )
        {
            LOGGER_FATAL( "catch exception and write dumb '%s'"
                , g_monitor->m_dumpPath
            );
        }
        else
        {
            LOGGER_FATAL( "catch exception and write dumb '%s'\n\n\n%s\n\n\n"
                , g_monitor->m_dumpPath
                , stack
            );
        }

        Win32CriticalErrorsMonitorPlugin::s_writeCrashDump( pExceptionPointers );
#endif

        return EXCEPTION_EXECUTE_HANDLER;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CriticalErrorsMonitorPlugin::_initializePlugin()
    {
        Char userPath[MENGINE_MAX_PATH] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserPath( userPath );

        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        m_dumpPath.format( "%sDump_%u_%u_%u_%u_%u_%u.dmp"
            , userPath
            , dateTime.year
            , dateTime.month
            , dateTime.day
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        g_monitor = this;

        ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
        ::SetUnhandledExceptionFilter( &Win32CriticalErrorsMonitorPlugin::s_exceptionHandler );

        LOGGER_INFO( "system", "start critical errors monitor plugin" );
        LOGGER_INFO( "system", "dump path: '%s'"
            , m_dumpPath
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32CriticalErrorsMonitorPlugin::_finalizePlugin()
    {
        g_monitor = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
