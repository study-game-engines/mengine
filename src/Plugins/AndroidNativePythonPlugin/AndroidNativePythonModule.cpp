#include "AndroidNativePythonModule.h"

#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/AndroidPlatformExtensionInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"

#include "Environment/Android/AndroidUtils.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
static Mengine::AndroidNativePythonModule * s_androidNativePythonModule = nullptr;
//////////////////////////////////////////////////////////////////////////
static jmethodID jmethodID_initializePlugins;
//////////////////////////////////////////////////////////////////////////
extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1setupPythonJNI )(JNIEnv * env, jclass cls)
    {
        jmethodID_initializePlugins = env->GetMethodID( cls, "pythonInitializePlugins", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1call )(JNIEnv * env, jclass cls, jstring _plugin, jstring _method, int _id, jstring _args)
    {
        if( s_androidNativePythonModule != nullptr )
        {
            const Mengine::Char * plugin_str = env->GetStringUTFChars( _plugin, nullptr );
            const Mengine::Char * method_str = env->GetStringUTFChars( _method, nullptr );
            const Mengine::Char * args_str = env->GetStringUTFChars( _args, nullptr );

            Mengine::String plugin = plugin_str;
            Mengine::String method = method_str;
            Mengine::String args = args_str;

            env->ReleaseStringUTFChars( _plugin, plugin_str );
            env->ReleaseStringUTFChars( _method, method_str );
            env->ReleaseStringUTFChars( _args, args_str );

            s_androidNativePythonModule->addCommand( [plugin, method, _id, args]( const Mengine::PythonEventHandlerPtr & _handler )
            {
                _handler->pythonMethod( plugin, method, _id, args );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1addPlugin )(JNIEnv * env, jclass cls, jstring _name, jobject _plugin)
    {
        if( s_androidNativePythonModule != nullptr )
        {
            const Mengine::Char * name_str = env->GetStringUTFChars( _name, nullptr );

            Mengine::String name = name_str;

            env->ReleaseStringUTFChars( _name, name_str );

            jobject new_plugin = env->NewGlobalRef( _plugin );

            s_androidNativePythonModule->addCommand( [name, new_plugin]( const Mengine::PythonEventHandlerPtr & _handler )
            {
                _handler->addPlugin( name, new_plugin );
            } );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonModule::AndroidNativePythonModule()
        : m_jenv( nullptr )
        , m_kernel( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonModule::~AndroidNativePythonModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::_initializeModule()
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
            ->getKernel();

        pybind::def_functor( kernel, "setAndroidCallback", this, &AndroidNativePythonModule::setAndroidCallback );
        pybind::def_functor_args( kernel, "androidMethod", this, &AndroidNativePythonModule::androidMethod );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation.initialize( mutex ) == false )
        {
            return false;
        }

        m_globals = pybind::make_dict_t( kernel );

        m_kernel = kernel;

        s_androidNativePythonModule = this;

        m_eventation.setEventHandler( PythonEventHandlerPtr::from( this ) );

        AndroidPlatformExtensionInterface * extension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        m_jenv = extension->getJENV();
        jobject jActivity = extension->getJObjectActivity();

        m_jenv->CallVoidMethod( jActivity, jmethodID_initializePlugins );

        m_eventation.invoke();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::_finalizeModule()
    {
        m_globals.reset();

        s_androidNativePythonModule = nullptr;

        m_callbacks.clear();

        m_eventation.finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::addCommand( const LambdaPythonEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::_update( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::pythonMethod( const String & _plugin, const String & _method, int _id, const String & _args )
    {
        m_eventation.invoke();

        ConstString plugin_c = Helper::stringizeString( _plugin );
        ConstString method_c = Helper::stringizeString( _method );

        MapAndroidCallbacks::iterator it_found = m_callbacks.find( Helper::makePair( plugin_c, method_c ) );

        if( it_found == m_callbacks.end() )
        {
            return;
        }

        const Char * args_str = _args.c_str();

        PyObject * py_args = m_kernel->eval_string( args_str, m_globals.ptr(), nullptr );

        const pybind::object & cb = it_found->second;

        pybind::object py_result = cb.call_native( pybind::tuple( m_kernel, py_args, pybind::borrowed ) );

        MENGINE_ASSERTION_FATAL( py_result != nullptr, "android plugin '%s' method '%s' id '%d' invalid call"
            , _plugin.c_str()
            , _method.c_str()
            , _id
        );

        MENGINE_ASSERTION_FATAL( !(py_result.is_none() == true && _id != 0), "android plugin '%s' method '%s' return 'None' but have callback '%d'"
            , _plugin.c_str()
            , _method.c_str()
            , _id
        );

        MENGINE_ASSERTION_FATAL( !(py_result.is_none() == false && _id == 0), "android plugin '%s' method '%s' return '%s' but not setup callback"
            , _plugin.c_str()
            , _method.c_str()
            , py_result.str().c_str()
        );

        if( _id == 0 )
        {
            return;
        }

        this->androidResponse( _id, py_result );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::addPlugin( const String & _name, const jobject & _plugin )
    {
        m_plugins.emplace( Helper::stringizeString( _name ), _plugin );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb )
    {
        m_callbacks.emplace( Helper::makePair( _plugin, _method ), _cb );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::androidResponse( int _id, const pybind::object & _result ) const
    {
        m_eventation.invoke();

        jclass cls = m_jenv->FindClass( "java/lang/Object" );

        jobject jresult;

        if( _result.is_none() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( cls, "<init>", "()V" );
            jresult = m_jenv->NewObject( cls, constructor );
        }
        else if( _result.is_bool() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( cls, "<init>", "(Z)V" );
            jresult = m_jenv->NewObject( cls, constructor, (bool)_result.extract() );
        }
        else if( _result.is_integer() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( cls, "<init>", "(I)V" );
            jresult = m_jenv->NewObject( cls, constructor, (int32_t)_result.extract() );
        }
        else if( _result.is_long() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( cls, "<init>", "(L)V" );
            jresult = m_jenv->NewObject( cls, constructor, (int64_t)_result.extract() );
        }
        else if( _result.is_float() == true )
        {
            static jmethodID constructor = m_jenv->GetMethodID( cls, "<init>", "(F)V" );
            jresult = m_jenv->NewObject( cls, constructor, (float)_result.extract() );
        }
        else if( _result.is_string() == true )
        {
            const Char * value_str = (const Char *)_result.extract();

            jstring jvalue = m_jenv->NewStringUTF( value_str );

            static jmethodID constructor = m_jenv->GetMethodID( cls, "<init>", "(Ljava/lang/String;)V" );
            jresult = m_jenv->NewObject( cls, constructor, jvalue );

            m_jenv->DeleteLocalRef( jvalue );
        }
        else
        {
            LOGGER_ERROR( "android response call '%d' unsupported result type [%s]"
                , _id
                , _result.repr_type().c_str()
            );

            return false;
        }

        AndroidPlatformExtensionInterface * extension = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        jclass jclass_activity = extension->getJClassActivity();

        static jmethodID jmethodID_method = m_jenv->GetMethodID( jclass_activity, "responseCall", "(ILjava/lang/Object;)V" );

        MENGINE_ASSERTION_FATAL( jmethodID_method != 0, "android activity not found method 'responseCall'" );

        jobject jobject_activity = extension->getJObjectActivity();

        m_jenv->CallVoidMethod( jobject_activity, jmethodID_method, _id, jresult );

        m_eventation.invoke();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        MENGINE_ASSERTION_FATAL( _args.size() <= 32, "android method plugin '%s' method '%s' max args [32 < %u]"
            , _args.size()
        );

        m_eventation.invoke();

        Char signature[1024] = {'\0'};

        MENGINE_STRCAT( signature, "(" );

        jvalue jargs[32];
        MENGINE_MEMSET( jargs, 0, sizeof( jvalue ) * 32 );

        jstring jfree[32];

        uint32_t index_args = 0;
        uint32_t index_free = 0;

        for( const pybind::object & arg : _args )
        {
            if( arg.is_none() == true )
            {
                jargs[index_args++].l = nullptr;

                MENGINE_STRCAT( signature, "L" );
            }
            else if( arg.is_bool() == true )
            {
                jboolean jvalue = (bool)arg.extract();
                jargs[index_args++].z = jvalue;

                MENGINE_STRCAT( signature, "Z" );
            }
            else if( arg.is_integer() == true )
            {
                jint jvalue = (int32_t)arg.extract();
                jargs[index_args++].i = jvalue;

                MENGINE_STRCAT( signature, "I" );
            }
            else if( arg.is_long() == true )
            {
                jlong jvalue = (int64_t)arg.extract();
                jargs[index_args++].j = jvalue;

                MENGINE_STRCAT( signature, "J" );
            }
            else if( arg.is_float() == true )
            {
                jfloat jvalue = (float)arg.extract();
                jargs[index_args++].f = jvalue;

                MENGINE_STRCAT( signature, "F" );
            }
            else if( arg.is_string() == true )
            {
                const Char * value_str = (const Char *)arg.extract();

                jstring jvalue = m_jenv->NewStringUTF( value_str );

                jargs[index_args++].l = jvalue;
                jfree[index_free++] = jvalue;

                MENGINE_STRCAT( signature, "Ljava/lang/String;" );
            }
            else
            {
                LOGGER_ERROR( "android plugin '%s' method '%s' unsupported arg [%u] type [%s]"
                    , _plugin.c_str()
                    , _method.c_str()
                    , index_args
                    , m_kernel->object_repr_type( arg.ptr() ).c_str()
                );

                return false;
            }
        }

        MENGINE_STRCAT( signature, ")V" );

        MapAndroidPlugins::const_iterator it_found = m_plugins.find( _plugin );

        if( it_found == m_plugins.end() )
        {
            LOGGER_ERROR( "android not found plugin '%s'"
                , _plugin.c_str()
            );

            return false;
        }

        jobject plugin = it_found->second;

        jclass plugin_class = m_jenv->GetObjectClass( plugin );

        jmethodID jmethodID_method = m_jenv->GetMethodID( plugin_class, _method.c_str(), signature );

        if( jmethodID_method == 0 )
        {
            LOGGER_ERROR( "android plugin '%s' not found method '%s' with signature '%s'"
                , _plugin.c_str()
                , _method.c_str()
                , signature
            );

            return false;
        }

        m_jenv->CallVoidMethodA( plugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != index_free; ++index )
        {
            jstring j = jfree[index];

            m_jenv->DeleteLocalRef( j );
        }

        m_eventation.invoke();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
