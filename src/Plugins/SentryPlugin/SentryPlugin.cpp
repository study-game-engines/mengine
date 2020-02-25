#include "SentryPlugin.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/NotificatorInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Stringalized.h"
#include "Kernel/Logger.h"

#include "Config/GitSHA1.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Sentry, Mengine::SentryPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::SentryPlugin()
        : m_options( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::~SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryPlugin::_initializePlugin()
    {
        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true )
        {
            return true;
        }

        const Char * sentryDSN = CONFIG_VALUE( "Sentry", "DSN", "" );

        if( ::strcmp( sentryDSN, "" ) == 0 )
        {
            LOGGER_WARNING( "Sentry don't setup DSN" );

            return true;
        }

        const Char * sentryHandler = CONFIG_VALUE( "Sentry", "Handler", "crashpad_handler.exe" );

        sentry_options_t * options = sentry_options_new();
        sentry_options_set_dsn( options, sentryDSN );
        sentry_options_set_handler_path( options, sentryHandler );
        sentry_options_set_system_crash_reporter_enabled( options, 1 );
        sentry_options_set_debug( options, 1 );
        sentry_init( options );

        m_options = options;

        sentry_set_extra( "Application", sentry_value_new_string( "Mengine" ) );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, this, &SentryPlugin::notifyCreateApplication_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::_finalizePlugin()
    {
        if( PLATFORM_SERVICE()
            ->isDebuggerPresent() == true )
        {
            return;
        }

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );

        sentry_shutdown();
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::notifyCreateApplication_()
    {
        Char companyName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME];
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        sentry_set_extra( "Company", sentry_value_new_string( companyName ) );

        Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME];
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        sentry_set_extra( "Project", sentry_value_new_string( projectName ) );

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        Char projectVersionString[32];
        Helper::stringalized( projectVersion, projectVersionString, 32 );

        sentry_set_extra( "Version", sentry_value_new_string( projectVersionString ) );

#ifdef MENGINE_GIT_SHA1
        sentry_set_extra( "Git", sentry_value_new_string( MENGINE_GIT_SHA1 ) );
#endif

        Char releaseString[32];
        sprintf( releaseString, "%s@%u", projectName, projectVersion );

        sentry_options_set_release( m_options, releaseString );

        //sentry_capture_event( sentry_value_new_message_event(
        //    /*   level */ SENTRY_LEVEL_INFO,
        //    /*  logger */ "custom",
        //    /* message */ "It works!"
        //) );

        //Sleep( 1000 );

        //*(int *)(0) = 0;
    }
}
