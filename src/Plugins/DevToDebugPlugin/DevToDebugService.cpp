#include "DevToDebugService.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/LoggerServiceInterface.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "Environment/Android/AndroidUtils.h"
#endif

#include "DevToDebugTab.h"

#include "DevToDebugPropertyConstBoolean.h"
#include "DevToDebugPropertyGetterBoolean.h"
#include "DevToDebugPropertyConstString.h"
#include "DevToDebugPropertyGetterString.h"
#include "DevToDebugPropertyConstColor.h"
#include "DevToDebugPropertyGetterColor.h"
#include "DevToDebugPropertyContent.h"

#include "DevToDebugWidget.h"
#include "DevToDebugWidgetText.h"
#include "DevToDebugWidgetButton.h"
#include "DevToDebugWidgetCheckbox.h"
#include "DevToDebugWidgetCommandLine.h"
#include "DevToDebugWidgetRadioButton.h"
#include "DevToDebugWidgetSelector.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "DevToDebugScriptEmbedding.h"
#endif

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/DocumentHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#if defined(MENGINE_PLATFORM_ANDROID)
//////////////////////////////////////////////////////////////////////////
extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidDevDebuggerMengine_1createDevTab )(JNIEnv * env, jclass cls, jstring _tab)
    {
        const Mengine::Char * tab_str = env->GetStringUTFChars( _tab, nullptr );

        Mengine::ConstString tab = Mengine::Helper::stringizeString( tab_str );

        env->ReleaseStringUTFChars( _tab, tab_str );

        if( DEVTODEBUG_SERVICE()
            ->hasTab( tab ) == false )
        {
            LOGGER_ERROR("invalid create dev tab has already exist '%s'"
                , tab.c_str()
            );

            return JNI_FALSE;
        }

        Mengine::DevToDebugTabInterfacePtr t = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugTab" ), MENGINE_DOCUMENT_FUNCTION );

        DEVTODEBUG_SERVICE()
                ->addTab( STRINGIZE_STRING_LOCAL( "Platform" ), t );

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidDevDebuggerMengine_1addDevButton )(JNIEnv * env, jclass cls, jstring _tab, jstring _id, jstring _title, jobject _cb)
    {
        const Mengine::Char * tab_str = env->GetStringUTFChars( _tab, nullptr );
        const Mengine::Char * id_str = env->GetStringUTFChars( _id, nullptr );
        const Mengine::Char * title_str = env->GetStringUTFChars( _title, nullptr );

        Mengine::ConstString tab = Mengine::Helper::stringizeString( tab_str );
        Mengine::ConstString id = Mengine::Helper::stringizeString( id_str );
        Mengine::String title = title_str;

        env->ReleaseStringUTFChars( _tab, tab_str );
        env->ReleaseStringUTFChars( _id, id_str );
        env->ReleaseStringUTFChars( _title, title_str );

        const Mengine::DevToDebugTabInterfacePtr & t = DEVTODEBUG_SERVICE()
                ->getTab( tab );

        Mengine::DevToDebugWidgetInterfacePtr button = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FUNCTION );

        button->setId( id );

        Mengine::DevToDebugPropertyInterfacePtr button_title = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FUNCTION );

        Mengine::UnknownDevToDebugPropertyConstStringInterfacePtr unknown_button_title = button_title->getUnknown();
        unknown_button_title->setValue( "Change" );

        button->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), button_title );

        Mengine::UnknownDevToDebugWidgetButtonInterfacePtr unknown_button = button->getDynamicUnknown();

        jclass cls_cb = env->GetObjectClass( _cb );

        jmethodID jmethodId_callback = env->GetMethodID( cls_cb, "callback", "()V" );

        jobject jcb = env->NewGlobalRef(_cb);

        unknown_button->setClickEvent( [env, jcb, jmethodId_callback]()
                                           {
                                               env->CallVoidMethod(jcb, jmethodId_callback);
                                           } );

        t->addWidget( button );
    }
    ///////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DevToDebugService, Mengine::DevToDebugService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugService::DevToDebugService()
        : m_status( EDTDS_NONE )
        , m_revision( 0 )
        , m_timerId( INVALID_UNIQUE_ID )
        , m_invalidateTabs( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugService::~DevToDebugService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & DevToDebugService::requiredServices() const
    {
        static ServiceRequiredList required = {
            cURLServiceInterface::getStaticServiceID()
            , ThreadSystemInterface::getStaticServiceID()
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugService::_initializeService()
    {
        const Char * DevToDebug_DSN = CONFIG_VALUE( "DevToDebugPlugin", "DSN", "" );

        if( MENGINE_STRCMP( DevToDebug_DSN, "" ) == 0 )
        {
            LOGGER_WARNING( "DevToDebug don't setup DSN" );

            return false;
        }

        m_dsn = DevToDebug_DSN;

        LOGGER_MESSAGE( "DevToDebug DSN: %s"
            , m_dsn.c_str()
        );

        m_mutexTabs = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_mutexCommands = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE, &DevToDebugService::notifyBootstrapperRunComplete_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, &DevToDebugService::notifyBootstrapperFinalizeGame_, MENGINE_DOCUMENT_FACTORABLE );

        if( Helper::addObjectPrototype<DevToDebugTab, 16>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugTab" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyConstBoolean, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstBoolean" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyGetterBoolean, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyGetterBoolean" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyConstString, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyGetterString, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyGetterString" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyConstColor, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstColor" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyGetterColor, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyGetterColor" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyContent, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyContent" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetButton, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetText, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetCheckbox, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetCheckbox" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetCommandLine, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetCommandLine" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetRadioButton, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetRadioButton" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetSelector, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetSelector" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        uint32_t DevToDebug_ProccesTime = CONFIG_VALUE( "DevToDebugPlugin", "ProccesTime", 500 );

        Helper::createSimpleThreadWorker( STRINGIZE_STRING_LOCAL( "DevToDebugProcess" ), ETP_BELOW_NORMAL, DevToDebug_ProccesTime, nullptr, [this]()
        {
            this->process();
        }, MENGINE_DOCUMENT_FACTORABLE );

        float DevToDebug_PropertySyncTime = CONFIG_VALUE( "DevToDebugPlugin", "PropertySyncTime", 500.f );

        UniqueId timerId = PLATFORM_SERVICE()
            ->addTimer( DevToDebug_PropertySyncTime, [this]( UniqueId _id )
        {
            MENGINE_UNUSED( _id );

            this->sync();
        }, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_FATAL( timerId != INVALID_UNIQUE_ID );

        m_timerId = timerId;

        DevToDebugLoggerPtr logger = Helper::makeFactorableUnique<DevToDebugLogger>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( logger );

        logger->setVerboseLevel( LM_VERBOSE );
        logger->setWriteHistory( true );

        LOGGER_SERVICE()
            ->registerLogger( logger );

        m_logger = logger;

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "DevToDebugScriptEmbedding" ), Helper::makeFactorableUnique<DevToDebugScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "DevToDebugScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::_finalizeService()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        if( m_logger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_logger );

            m_logger = nullptr;
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugTab" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstBoolean" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstColor" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyGetterBoolean" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyGetterString" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyGetterColor" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyContent" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetCheckbox" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetCommandLine" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetRadioButton" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetSelector" ), nullptr );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::_stopService()
    {
        Helper::destroySimpleThreadWorker( STRINGIZE_STRING_LOCAL( "DevToDebugProcess" ) );

        if( m_timerId != INVALID_UNIQUE_ID )
        {
            PLATFORM_SERVICE()
                ->removeTimer( m_timerId );
            m_timerId = INVALID_UNIQUE_ID;
        }

        m_mutexTabs = nullptr;
        m_mutexCommands = nullptr;

        m_tabsProcess.clear();
        m_tabsSync.clear();

        m_tabsCommands.clear();

        this->stop();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::addTab( const ConstString & _name, const DevToDebugTabInterfacePtr & _tab )
    {
        m_mutexTabs->lock();
        MENGINE_ASSERTION_FATAL( m_tabsProcess.exist( _name ) == false, "alredy exist devtodebug tab '%s'"
            , _name.c_str()
        );

        m_tabsProcess.emplace( _name, _tab );

        m_invalidateTabs = true;
        m_mutexTabs->unlock();

        m_tabsSync.emplace( _name, _tab );
    }
    //////////////////////////////////////////////////////////////////////////
    const DevToDebugTabInterfacePtr & DevToDebugService::getTab( const ConstString & _name ) const
    {
        const DevToDebugTabInterfacePtr & tab = m_tabsSync.find( _name );

        return tab;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugService::hasTab( const ConstString & _name ) const
    {
        bool result = m_tabsSync.exist( _name );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::removeTab( const ConstString & _name )
    {
        m_mutexTabs->lock();
        DevToDebugTabInterfacePtr tab = m_tabsProcess.erase( _name );

        m_invalidateTabs = true;
        m_mutexTabs->unlock();

        tab->finalize();

        m_tabsSync.erase( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::sync()
    {
        for( const HashtableDevToDebugTabs::value_type & value : m_tabsSync )
        {
            const DevToDebugTabInterfacePtr & tab = value.element;

            tab->syncWidgets();
        }

        m_mutexCommands->lock();
        for( const DevToDebugWidgetCommand & command : m_tabsCommands )
        {
            command();
        }
        m_tabsCommands.clear();
        m_mutexCommands->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::process()
    {
        switch( m_status )
        {
        case EDTDS_NONE:
            break;
        case EDTDS_READY:
            {
                m_status = EDTDS_CONNECTING;

                cURLHeaders headers;
                headers.push_back( "Content-Type:application/json" );

                jpp::object j = this->makeJsonConnectData();

                String data;
                Helper::writeJSONStringCompact( j, &data );

                HttpRequestID id = CURL_SERVICE()
                    ->headerData( m_dsn, headers, MENGINE_CURL_TIMEOUT_INFINITY, false, data, cURLReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_UNUSED( id );
            }break;
        case EDTDS_CONNECT:
            {
                cURLHeaders headers;
                headers.push_back( "Content-Type:application/json" );

                jpp::object j = this->makeJsonProcessData();

                String data;
                Helper::writeJSONStringCompact( j, &data );

                HttpRequestID id = CURL_SERVICE()
                    ->headerData( m_workerURL, headers, MENGINE_CURL_TIMEOUT_INFINITY, false, data, cURLReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_UNUSED( id );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::onHttpRequestComplete( const cURLResponseData & _response )
    {
        switch( m_status )
        {
        case Mengine::EDTDS_NONE:
            break;
        case Mengine::EDTDS_CONNECTING:
            {
                if( _response.successful == false )
                {
                    LOGGER_ERROR( "[DevToDebug] Connecting error: %s [code %u] [id %u]"
                        , _response.error.c_str()
                        , _response.code
                        , _response.id
                    );

                    this->stop();

                    break;
                }

                if( _response.code / 100 != 2 )
                {
                    LOGGER_ERROR( "[DevToDebug] Connecting error: %s data: %s [code %u] [id %u]"
                        , _response.error.c_str()
                        , _response.data.c_str()
                        , _response.code
                        , _response.id
                    );

                    this->stop();

                    break;
                }

                jpp::object j = Helper::loadJSONStreamFromString( _response.data, MENGINE_DOCUMENT_FACTORABLE );

                m_workerURL = j.get( "worker_url", "" );

                if( m_workerURL.empty() == true )
                {
                    LOGGER_ERROR( "[DevToDebug] Connecting response error: %s [code %u] [id %u]"
                        , _response.data.c_str()
                        , _response.code
                        , _response.id
                    );

                    this->stop();

                    break;
                }

                if( m_logger != nullptr )
                {
                    String loggerURL = m_workerURL;
                    loggerURL += "log";
                    loggerURL += "/";

                    m_logger->setLoggerURL( loggerURL );
                }

                LOGGER_INFO( "devtodebug", "Request Connect: %s [id %u]"
                    , m_workerURL.c_str()
                    , _response.id
                );

                m_status = EDTDS_CONNECT;
            }break;
        case Mengine::EDTDS_CONNECT:
            {
                if( _response.successful == false )
                {
                    LOGGER_ERROR( "[DevToDebug] Connect response error: %s [code %u] [id %u]"
                        , _response.error.c_str()
                        , _response.code
                        , _response.id
                    );

                    this->stop();

                    break;
                }

                if( _response.code / 100 != 2 )
                {
                    LOGGER_ERROR( "[DevToDebug] Connect response error: %s data: %s [code %u] [id %u]"
                        , _response.error.c_str()
                        , _response.data.c_str()
                        , _response.code
                        , _response.id
                    );

                    this->stop();

                    break;
                }

                jpp::object j = Helper::loadJSONStreamFromString( _response.data, MENGINE_DOCUMENT_FACTORABLE );

                uint32_t revision_from = j.get( "revision_from", 0U );
                uint32_t revision_to = j.get( "revision_to", 0U );

                if( m_revision != revision_from )
                {
                    LOGGER_ERROR( "[DevToDebug] Connect out of sync revision %u != from %u: %s [code %u] [id %u]"
                        , m_revision
                        , revision_from
                        , _response.data.c_str()
                        , _response.code
                        , _response.id
                    );

                    this->stop();

                    break;
                }

                if( revision_from == revision_to )
                {
                    break;
                }

#ifdef MENGINE_DEBUG
                String data;
                Helper::writeJSONStringCompact( j, &data );

                LOGGER_INFO( "devtodebug", "Request Process: %s [id %u]"
                    , data.c_str()
                    , _response.id
                );
#endif

                m_revision = revision_to;

                jpp::array activity = j.get( "activity" );

                for( const jpp::object & a : activity )
                {
                    ConstString tab_name = a.get( "tab_name" );
                    ConstString id = a.get( "id" );
                    jpp::object d = a.get( "data" );

                    m_mutexTabs->lock();
                    DevToDebugTabInterfacePtr tab = m_tabsProcess.find( tab_name );
                    m_mutexTabs->unlock();

                    if( tab == nullptr )
                    {
                        continue;
                    }

                    const DevToDebugWidgetInterfacePtr & widget = tab->findWidget( id );

                    DevToDebugWidgetPtr widget_base = DevToDebugWidgetPtr::dynamic_from( widget );

                    widget_base->process( d, m_mutexCommands, &m_tabsCommands );
                }
            }break;
        default:
            break;
        }

        return;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::stop()
    {
        m_status = EDTDS_NONE;
        m_revision = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeJsonTabs( bool _force, bool * const _invalidateTabs )
    {
        m_mutexTabs->lock();

        jpp::object jtabs = jpp::make_object();

        bool invalidateTabs = false;

        for( const HashtableDevToDebugTabs::value_type & value : m_tabsProcess )
        {
            const ConstString & key = value.key;
            const DevToDebugTabInterfacePtr & tab = value.element;

            jpp::object jtab = jpp::make_object();

            jpp::array jwidgets = jpp::make_array();

            bool invalidateWidgets;
            tab->foreachWidgets( [&jwidgets, _force]( const DevToDebugWidgetInterfacePtr & _widget )
            {
                DevToDebugWidgetPtr widget = DevToDebugWidgetPtr::dynamic_from( _widget );

                jpp::object jwidget = jpp::make_object();

                if( widget->fillJson( jwidget, _force ) == true )
                {
                    jwidgets.push_back( jwidget );
                }
            }, &invalidateWidgets );

            invalidateTabs |= invalidateWidgets;

            if( jwidgets.empty() == false )
            {
                jtab.set( "widgets", jwidgets );

                jtabs.set( key, jtab );
            }
        }

        *_invalidateTabs = invalidateTabs | m_invalidateTabs;

        m_invalidateTabs = false;

        m_mutexTabs->unlock();

        return jtabs;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeJsonConnectData()
    {
        jpp::object j = jpp::make_object();

        Char fingerprint[MENGINE_PLATFORM_FINGERPRINT_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getFingerprint( fingerprint );

        j.set( "did", fingerprint );

        Char userName[MENGINE_PLATFORM_USER_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserName( userName );

        j.set( "name", userName );

        LOGGER_INFO( "devtodebug", "Device did '%s' name '%s'"
            , fingerprint
            , userName
        );

        jpp::object jstate = jpp::make_object();

        bool invalidateTabs;
        jpp::object jtabs = this->makeJsonTabs( true, &invalidateTabs );

        jstate.set( "tabs", jtabs );

        j.set( "state", jstate );

        m_invalidateTabs = false;

        return j;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeJsonProcessData()
    {
        jpp::object j = jpp::make_object();

        j.set( "confirmed_revision", m_revision );

        jpp::object jstate = jpp::make_object();

        bool invalidateTabs;
        jpp::object jtabs = this->makeJsonTabs( false, &invalidateTabs );

        if( jtabs.empty() == false )
        {
            jstate.set( "tabs", jtabs );

            if( m_invalidateTabs == true || invalidateTabs == true )
            {
                j.set( "new_state", jstate );
            }
            else
            {
                j.set( "change_state", jstate );
            }
        }

#ifdef MENGINE_DEBUG
        static uint32_t old_revision = 0;

        if( m_revision != old_revision )
        {
            LOGGER_INFO( "devtodebug", "confirmed revision: %u"
                , m_revision
            );

            String data;
            Helper::writeJSONStringCompact( jstate, &data );

            LOGGER_INFO( "devtodebug", "change state: %s"
                , data.c_str()
            );

            old_revision = m_revision;
        }
#endif

        return j;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::notifyBootstrapperRunComplete_()
    {
        m_status = EDTDS_READY;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::notifyBootstrapperFinalizeGame_()
    {
        if( m_workerURL.empty() == true )
        {
            return;
        }

        Char delete_url[1024] = {'\0'};
        MENGINE_SNPRINTF( delete_url, 256, "%s/delete/"
            , m_workerURL.c_str()
        );

        cURLHeaders headers;
        headers.push_back( "Content-Type:application/json" );

        jpp::object j = jpp::make_object();

        String data;
        Helper::writeJSONStringCompact( j, &data );

        CURL_SERVICE()
            ->headerData( m_workerURL, headers, MENGINE_CURL_TIMEOUT_INFINITY, false, data, nullptr, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
}