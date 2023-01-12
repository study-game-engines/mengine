#include "AndroidNativePythonModule.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"

#include "Environment/Android/AndroidUtils.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
static Mengine::AndroidNativePythonModule * s_androidNativePythonModule = nullptr;
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1call )(JNIEnv * env, jclass cls, jstring _plugin, jstring _method, int _id, jobjectArray _args)
    {
        const Mengine::Char * plugin_str = env->GetStringUTFChars( _plugin, nullptr );
        const Mengine::Char * method_str = env->GetStringUTFChars( _method, nullptr );

        Mengine::String plugin = plugin_str;
        Mengine::String method = method_str;

        env->ReleaseStringUTFChars( _plugin, plugin_str );
        env->ReleaseStringUTFChars( _method, method_str );

        if( s_androidNativePythonModule == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android call plugin '%s' method '%s'"
                , plugin.c_str()
                , method.c_str()
            );

            return;
        }

        jobjectArray new_args = (jobjectArray)env->NewGlobalRef( _args );

        s_androidNativePythonModule->addCommand( [plugin, method, _id, new_args]( const Mengine::PythonEventHandlerInterfacePtr & _handler )
        {
            _handler->pythonMethod( plugin, method, _id, new_args );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1addPlugin )(JNIEnv * env, jclass cls, jstring _name, jobject _plugin)
    {
        const Mengine::Char * name_str = env->GetStringUTFChars( _name, nullptr );

        Mengine::String name = name_str;

        env->ReleaseStringUTFChars( _name, name_str );

        if( s_androidNativePythonModule == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android add plugin '%s'"
                , name.c_str()
            );

            return;
        }

        jobject new_plugin = env->NewGlobalRef( _plugin );

        s_androidNativePythonModule->addCommand( [name, new_plugin]( const Mengine::PythonEventHandlerInterfacePtr & _handler )
        {
            _handler->addPlugin( name, new_plugin );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativePython_1activateSemaphore )(JNIEnv * env, jclass cls, jstring _name)
    {
        const Mengine::Char * name_str = env->GetStringUTFChars( _name, nullptr );

        Mengine::String name = name_str;

        env->ReleaseStringUTFChars( _name, name_str );

        if( s_androidNativePythonModule == nullptr )
        {
            __android_log_print(ANDROID_LOG_ERROR, "Mengine", "invalid android activate semaphore '%s'"
                , name.c_str()
            );

            return;
        }

        s_androidNativePythonModule->addCommand( [name]( const Mengine::PythonEventHandlerInterfacePtr & _handler )
        {
            _handler->activateSemaphore( name );
        } );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonModule::AndroidNativePythonModule()
        : m_jclass_MengineActivity( nullptr )
        , m_jobject_MengineActivity( nullptr )
        , m_jclass_Object( nullptr )
        , m_jclass_Boolean( nullptr )
        , m_jclass_Character( nullptr )
        , m_jclass_Integer( nullptr )
        , m_jclass_Long( nullptr )
        , m_jclass_Float( nullptr )
        , m_jclass_Double( nullptr )
        , m_jclass_String( nullptr )
        , m_jclass_ArrayList( nullptr )
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
        pybind::def_functor_args( kernel, "androidBooleanMethod", this, &AndroidNativePythonModule::androidBooleanMethod );
        pybind::def_functor_args( kernel, "androidInteger32Method", this, &AndroidNativePythonModule::androidInteger32Method );
        pybind::def_functor_args( kernel, "androidInteger64Method", this, &AndroidNativePythonModule::androidInteger64Method );
        pybind::def_functor_args( kernel, "androidFloatMethod", this, &AndroidNativePythonModule::androidFloatMethod );
        pybind::def_functor_args( kernel, "androidStringMethod", this, &AndroidNativePythonModule::androidStringMethod );

        pybind::def_functor_args( kernel, "waitAndroidSemaphore", this, &AndroidNativePythonModule::waitAndroidSemaphore );

        m_eventation = Helper::makeFactorableUnique<PythonEventation>( MENGINE_DOCUMENT_FACTORABLE );

        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation->initialize( mutex ) == false )
        {
            return false;
        }

        m_kernel = kernel;

        s_androidNativePythonModule = this;

        m_eventation->setEventHandler( PythonEventHandlerInterfacePtr::from( this ) );

        ANDROID_ENVIRONMENT_SERVICE()
            ->addAndroidEventation( m_eventation );

        m_jclass_Object = ANDROID_ENVIRONMENT_SERVICE()
                ->getJClassObject();

        m_jclass_Boolean = ANDROID_ENVIRONMENT_SERVICE()
                ->getJClassBoolean();

        m_jclass_Character = ANDROID_ENVIRONMENT_SERVICE()
                ->getJClassCharacter();

        m_jclass_Integer = ANDROID_ENVIRONMENT_SERVICE()
                -> getJClassInteger();

        m_jclass_Long = ANDROID_ENVIRONMENT_SERVICE()
                -> getJClassLong();

        m_jclass_Float = ANDROID_ENVIRONMENT_SERVICE()
                -> getJClassFloat();

        m_jclass_Double = ANDROID_ENVIRONMENT_SERVICE()
                -> getJClassDouble();

        m_jclass_String = ANDROID_ENVIRONMENT_SERVICE()
                -> getJClassString();

        m_jclass_ArrayList = ANDROID_ENVIRONMENT_SERVICE()
                ->getJClassArrayList();

        m_jclass_MengineActivity = ANDROID_ENVIRONMENT_SERVICE()
                ->getJClassMengineActivity();

        m_jobject_MengineActivity = ANDROID_ENVIRONMENT_SERVICE()
                ->getJObjectMengineActivity();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jmethodID jmethodID_initializePlugins = jenv->GetMethodID( m_jclass_MengineActivity, "pythonInitializePlugins", "()V" );

        MENGINE_ASSERTION_FATAL( jmethodID_initializePlugins != nullptr, "invalid get android method 'pythonInitializePlugins'" );

        jenv->CallVoidMethod( m_jobject_MengineActivity, jmethodID_initializePlugins );

        m_eventation->invoke();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::_finalizeModule()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jmethodID jmethodID_pythonFinalizePlugins = jenv->GetMethodID( m_jclass_MengineActivity, "pythonFinalizePlugins", "()V" );

        MENGINE_ASSERTION_FATAL( jmethodID_pythonFinalizePlugins != nullptr, "invalid get android method 'pythonFinalizePlugins'" );

        jenv->CallVoidMethod( m_jobject_MengineActivity, jmethodID_pythonFinalizePlugins );

        s_androidNativePythonModule = nullptr;

        m_semaphoreListeners.clear();
        m_callbacks.clear();
        m_plugins.clear();

        ANDROID_ENVIRONMENT_SERVICE()
            ->removeAndroidEventation( m_eventation );

        m_eventation->finalize();
        m_eventation = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::addCommand( const LambdaPythonEventHandler & _command )
    {
        m_eventation->addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::_beginUpdate( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        m_eventation->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::_endUpdate()
    {
        m_eventation->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * AndroidNativePythonModule::getPythonAttribute( JNIEnv * _jenv, jobject obj )
    {
        PyObject * py_value = nullptr;

        if( obj == nullptr )
        {
            py_value = m_kernel->ret_none();
        }
        else if( _jenv->IsInstanceOf( obj, m_jclass_Boolean ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( m_jclass_Boolean, "booleanValue", "()Z" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Boolean [booleanValue] ()C'" );

            jboolean value = _jenv->CallBooleanMethod( obj, methodValue );

            py_value = m_kernel->ret_bool( value );
        }
        else if( _jenv->IsInstanceOf( obj, m_jclass_Character ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( m_jclass_Character, "charValue", "()C" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Character [charValue] ()C'" );

            jchar value = _jenv->CallCharMethod( obj, methodValue );

            Char value_str[2] = {(Char)value, '\0'};
            py_value = m_kernel->string_from_char_size( value_str, 1 );
        }
        else if( _jenv->IsInstanceOf( obj, m_jclass_Integer ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( m_jclass_Integer, "intValue", "()I" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Integer [intValue] ()I'" );

            jint value = _jenv->CallIntMethod( obj, methodValue );

            py_value = m_kernel->ptr_int32( value );
        }
        else if( _jenv->IsInstanceOf( obj, m_jclass_Long ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( m_jclass_Long, "longValue", "()J" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Long [longValue] ()J'" );

            jlong value = _jenv->CallLongMethod( obj, methodValue );

            py_value = m_kernel->ptr_int64( value );
        }
        else if( _jenv->IsInstanceOf( obj, m_jclass_Float ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( m_jclass_Float, "floatValue", "()F" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Float [floatValue] ()F'" );

            jfloat value = _jenv->CallFloatMethod( obj, methodValue );

            py_value = m_kernel->ptr_float( value );
        }
        else if( _jenv->IsInstanceOf( obj, m_jclass_Double ) == JNI_TRUE )
        {
            static jmethodID methodValue = _jenv->GetMethodID( m_jclass_Double, "doubleValue", "()D" );

            MENGINE_ASSERTION_FATAL( methodValue != nullptr, "invalid get android method 'java/lang/Double [doubleValue] ()D'" );

            jfloat value = _jenv->CallDoubleMethod( obj, methodValue );

            py_value = m_kernel->ptr_float( value );
        }
        else if( _jenv->IsInstanceOf( obj, m_jclass_String ) == JNI_TRUE )
        {
            const Char * obj_str = _jenv->GetStringUTFChars( (jstring)obj, NULL);

            py_value = m_kernel->string_from_char( obj_str );

            _jenv->ReleaseStringUTFChars( (jstring)obj, obj_str );
        }
        else if( _jenv->IsInstanceOf( obj, m_jclass_ArrayList ) == JNI_TRUE )
        {
            static jfieldID fieldElementData = _jenv->GetFieldID( m_jclass_ArrayList, "elementData", "[Ljava/lang/Object;" );

            MENGINE_ASSERTION_FATAL( fieldElementData != nullptr );

            jobjectArray list_elementData = (jobjectArray)_jenv->GetObjectField( obj, fieldElementData );

            jsize list_size = _jenv->GetArrayLength( list_elementData );

            PyObject * py_list = m_kernel->tuple_new( list_size );

            for( jsize index = 0; index != list_size; ++index )
            {
                jobject list_obj = _jenv->GetObjectArrayElement( list_elementData, index );

                PyObject * py_obj = this->getPythonAttribute( _jenv, list_obj );

                MENGINE_ASSERTION_FATAL( py_obj != nullptr );

                m_kernel->tuple_setitem( py_list, index, py_obj );
            }

            py_value = py_list;
        }
        else
        {
            jclass cls_obj = _jenv->GetObjectClass( obj );

            jmethodID mid_getClass = _jenv->GetMethodID( cls_obj, "getClass", "()Ljava/lang/Class;" );
            jobject obj_class = _jenv->CallObjectMethod( obj, mid_getClass );

            jclass cls_class = _jenv->GetObjectClass( obj_class );

            jmethodID mid_getName = _jenv->GetMethodID( cls_class, "getName", "()Ljava/lang/String;" );

            jstring obj_class_name = (jstring)_jenv->CallObjectMethod( obj_class, mid_getName );

            const Char * obj_class_name_str = _jenv->GetStringUTFChars( obj_class_name, NULL);

            LOGGER_ERROR( "unsupported java argument type '%s'"
                , obj_class_name_str
            );

            _jenv->ReleaseStringUTFChars( obj_class_name, obj_class_name_str );
            _jenv->DeleteLocalRef( obj_class_name );

            _jenv->DeleteLocalRef( obj_class );
        }

        return py_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::pythonMethod( const String & _plugin, const String & _method, int32_t _id, jobjectArray _args )
    {
        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        ConstString plugin_c = Helper::stringizeString( _plugin );
        ConstString method_c = Helper::stringizeString( _method );

        LOGGER_INFO( "android", "call python plugin '%s' method '%s' [%s]"
            , _plugin.c_str()
            , _method.c_str()
            , (m_callbacks.find( Helper::makePair( plugin_c, method_c ) ) != m_callbacks.end() ? "Found" : "NOT-FOUND")
        );

        MapAndroidCallbacks::const_iterator it_found = m_callbacks.find( Helper::makePair( plugin_c, method_c ) );

        if( it_found == m_callbacks.end() )
        {
            jenv->DeleteGlobalRef( _args );

            return;
        }

        jsize args_size = jenv->GetArrayLength( _args );

        PyObject * py_args = m_kernel->tuple_new( args_size );

        for( jsize index = 0; index != args_size; ++index )
        {
            jobject obj = jenv->GetObjectArrayElement( _args, index );

            PyObject * py_arg = this->getPythonAttribute( jenv, obj );

            MENGINE_ASSERTION_FATAL( py_arg != nullptr, "android plugin '%s' method '%s' id '%d' invalid arg"
                , _plugin.c_str()
                , _method.c_str()
                , _id
            );

            m_kernel->tuple_setitem( py_args, index, py_arg );
        }

        jenv->DeleteGlobalRef( _args );

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

        this->androidResponse( jenv, _id, py_result );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::addPlugin( const String & _name, const jobject & _plugin )
    {
        MENGINE_ASSERTION_FATAL( m_plugins.find( Helper::stringizeString( _name ) ) == m_plugins.end(), "invalid add plugin '%s' [double]"
            , _name.c_str()
        );

        SCRIPT_SERVICE()
            ->setAvailablePlugin( _name.c_str(), true );

        m_plugins.emplace( Helper::stringizeString( _name ), _plugin );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb )
    {
        MENGINE_ASSERTION_FATAL( m_callbacks.find( Helper::makePair( _plugin, _method ) ) == m_callbacks.end(), "invalid add plugin '%s' callback '%s' [double]"
            , _plugin.c_str()
            , _method.c_str()
        );

        m_callbacks.emplace( Helper::makePair( _plugin, _method ), _cb );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::androidResponse( JNIEnv * _jenv, int32_t _id, const pybind::object & _result ) const
    {
        m_eventation->invoke();

        jobject jresult;

        if( _result.is_none() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( m_jclass_Object, "<init>", "()V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] ()V'" );

            jresult = _jenv->NewObject( m_jclass_Object, constructor );
        }
        else if( _result.is_bool() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( m_jclass_Boolean, "<init>", "(Z)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (Z)V'" );

            jresult = _jenv->NewObject( m_jclass_Boolean, constructor, (bool)_result.extract() );
        }
        else if( _result.is_integer() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( m_jclass_Integer, "<init>", "(I)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (I)V'" );

            jresult = _jenv->NewObject( m_jclass_Integer, constructor, (int32_t)_result.extract() );
        }
        else if( _result.is_long() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( m_jclass_Long, "<init>", "(L)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (L)V'" );

            jresult = _jenv->NewObject( m_jclass_Long, constructor, (int64_t)_result.extract() );
        }
        else if( _result.is_float() == true )
        {
            static jmethodID constructor = _jenv->GetMethodID( m_jclass_Float, "<init>", "(F)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (F)V'" );

            jresult = _jenv->NewObject( m_jclass_Float, constructor, (float)_result.extract() );
        }
        else if( _result.is_string() == true )
        {
            const Char * value_str = (const Char *)_result.extract();

            jstring jvalue = _jenv->NewStringUTF( value_str );

            static jmethodID constructor = _jenv->GetMethodID( m_jclass_String, "<init>", "(Ljava/lang/String;)V" );

            MENGINE_ASSERTION_FATAL( constructor != nullptr, "invalid get android method 'java/lang/Object [<init>] (Ljava/lang/String;)V'" );

            jresult = _jenv->NewObject( m_jclass_String, constructor, jvalue );

            _jenv->DeleteLocalRef( jvalue );
        }
        else
        {
            LOGGER_ERROR( "android response call '%d' unsupported result type [%s]"
                , _id
                , _result.repr_type().c_str()
            );

            return false;
        }

        static jmethodID jmethodID_pythonCallResponse = _jenv->GetMethodID(m_jclass_MengineActivity, "pythonCallResponse", "(ILjava/lang/Object;)V" );

        MENGINE_ASSERTION_FATAL( jmethodID_pythonCallResponse != 0, "android activity not found method 'Activity [pythonCallResponse] (ILjava/lang/Object;)V'" );

        _jenv->CallVoidMethod(m_jobject_MengineActivity, jmethodID_pythonCallResponse, _id, jresult );

        _jenv->DeleteLocalRef( jresult );

        m_eventation->invoke();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [void]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "V", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return;
        }

        jenv->CallVoidMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [boolean]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Z", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return false;
        }

        jboolean jresult = jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (bool)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidNativePythonModule::androidInteger32Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [int]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "I", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0;
        }

        jint jresult = jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (int32_t)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidNativePythonModule::androidInteger64Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [long]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "J", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0;
        }

        jlong jresult = jenv->CallBooleanMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (int64_t)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidNativePythonModule::androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [float]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "F", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return 0.f;
        }

        jfloat jresult = jenv->CallFloatMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        return (float)jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    String AndroidNativePythonModule::androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const
    {
        LOGGER_INFO( "android", "call android plugin '%s' method '%s' args '%s' [string]"
            , _plugin.c_str()
            , _method.c_str()
            , m_kernel->object_repr( _args.ptr() ).c_str()
        );

        m_eventation->invoke();

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jvalue jargs[32];
        jobject jfree[32];
        uint32_t freeCount;

        jobject jplugin;
        jmethodID jmethodID_method;
        if( this->getAndroidMethod( jenv, _plugin, _method, _args, "Ljava/lang/String;", jargs, jfree, &freeCount, &jplugin, &jmethodID_method ) == false )
        {
            return String();
        }

        jstring jresult = (jstring)jenv->CallObjectMethodA( jplugin, jmethodID_method, jargs );

        for( uint32_t index = 0; index != freeCount; ++index )
        {
            jobject j = jfree[index];

            jenv->DeleteLocalRef( j );
        }

        m_eventation->invoke();

        const Mengine::Char * result_str = jenv->GetStringUTFChars( jresult, nullptr );

        Mengine::String result = result_str;

        jenv->ReleaseStringUTFChars( jresult, result_str );
        jenv->DeleteLocalRef( jresult );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonModule::getAndroidMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jfree, uint32_t * const _freeCount, jobject * const _jplugin, jmethodID * const _jmethodId ) const
    {
        MENGINE_ASSERTION_FATAL( _args.size() <= 32, "android method plugin '%s' method '%s' max args [32 < %u]"
            , _plugin.c_str()
            , _method.c_str()
            , _args.size()
        );

        MapAndroidPlugins::const_iterator it_found = m_plugins.find( _plugin );

        if( it_found == m_plugins.end() )
        {
            LOGGER_ERROR( "android not register plugin '%s' (call method '%s' args '%s')"
                , _plugin.c_str()
                , _method.c_str()
                , m_kernel->object_repr( _args.ptr() ).c_str()
            );

            return false;
        }

        jobject jplugin = it_found->second;

        jclass plugin_class = _jenv->GetObjectClass( jplugin );

        if( jplugin == nullptr )
        {
            LOGGER_ERROR( "android not found java plugin '%s' (call method '%s' args '%s')"
                , _plugin.c_str()
                , _method.c_str()
                , m_kernel->object_repr( _args.ptr() ).c_str()
            );

            return false;
        }

        Char signature[1024] = {'\0'};

        MENGINE_STRCAT( signature, "(" );

        MENGINE_MEMSET( _jargs, 0, sizeof( jvalue ) * 32 );
        MENGINE_MEMSET( _jfree, 0, sizeof( jstring ) * 32 );

        uint32_t index_args = 0;
        uint32_t index_free = 0;

        for( const pybind::object & arg : _args )
        {
            if( arg.is_none() == true )
            {
                _jargs[index_args++].l = nullptr;

                MENGINE_STRCAT( signature, "L" );
            }
            else if( arg.is_bool() == true )
            {
                jboolean jvalue = (bool)arg.extract();
                _jargs[index_args++].z = jvalue;

                MENGINE_STRCAT( signature, "Z" );
            }
            else if( arg.is_integer() == true )
            {
                jint jvalue = (int32_t)arg.extract();
                _jargs[index_args++].i = jvalue;

                MENGINE_STRCAT( signature, "I" );
            }
            else if( arg.is_long() == true )
            {
                jlong jvalue = (int64_t)arg.extract();
                _jargs[index_args++].j = jvalue;

                MENGINE_STRCAT( signature, "J" );
            }
            else if( arg.is_float() == true )
            {
                jfloat jvalue = (float)arg.extract();
                _jargs[index_args++].f = jvalue;

                MENGINE_STRCAT( signature, "F" );
            }
            else if( arg.is_string() == true )
            {
                const Char * value_str = (const Char *)arg.extract();

                jstring jvalue = _jenv->NewStringUTF( value_str );

                _jargs[index_args++].l = jvalue;
                _jfree[index_free++] = jvalue;

                MENGINE_STRCAT( signature, "Ljava/lang/String;" );
            }
            else if( arg.is_list() == true )
            {
                pybind::list l = arg.extract();

                pybind::list::size_type s = l.size();

                static jmethodID jmethodID_List_constructor = _jenv->GetMethodID( m_jclass_ArrayList, "<init>", "(I)V" );

                MENGINE_ASSERTION_FATAL( jmethodID_List_constructor != nullptr, "invalid get android method 'java/lang/ArrayList [<init>] (I)V'" );

                jobject jlist = _jenv->NewObject( m_jclass_ArrayList, jmethodID_List_constructor, s);

                uint32_t index = 0;
                for( const pybind::object & o : l )
                {
                    const Char * o_str = o.extract();

                    jstring jelement = _jenv->NewStringUTF( o_str );

                    static jmethodID jmethodID_List_add = _jenv->GetMethodID( m_jclass_ArrayList, "add", "(Ljava/lang/Object;)Z" );

                    MENGINE_ASSERTION_FATAL( jmethodID_List_add != nullptr, "invalid get android method 'java/lang/ArrayList [add] (I)V'" );

                    jboolean result = _jenv->CallBooleanMethod( jlist, jmethodID_List_add, jelement );

                    _jenv->DeleteLocalRef( jelement );

                    ++index;
                }

                _jargs[index_args++].l = jlist;
                _jfree[index_free++] = jlist;

                MENGINE_STRCAT( signature, "Ljava/util/List;" );
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

        MENGINE_STRCAT( signature, ")" );
        MENGINE_STRCAT( signature, _retType );

        jmethodID jmethodId = _jenv->GetMethodID( plugin_class, _method.c_str(), signature );

        if( jmethodId == nullptr )
        {
            LOGGER_ERROR( "android plugin '%s' not found method '%s' with signature '%s'"
                , _plugin.c_str()
                , _method.c_str()
                , signature
            );

            return false;
        }

        *_freeCount = index_free;
        *_jplugin = jplugin;
        *_jmethodId = jmethodId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::waitAndroidSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
    {
        AndroidSemaphoreListenerDesc desc;
        desc.name = _name;
        desc.cb = _cb;
        desc.args = _args;

        m_semaphoreListeners.emplace_back( desc );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jmethodID jmethodID_waitAndroidSemaphore = jenv->GetMethodID( m_jclass_MengineActivity, "waitAndroidSemaphore", "(Ljava/lang/String;)V" );

        MENGINE_ASSERTION_FATAL( jmethodID_waitAndroidSemaphore != nullptr, "invalid get android method 'pythonInitializePlugins'" );

        const Char * name_str = _name.c_str();

        jstring name_jvalue = jenv->NewStringUTF( name_str );

        jenv->CallVoidMethod( m_jobject_MengineActivity, jmethodID_waitAndroidSemaphore, name_jvalue );

        jenv->DeleteLocalRef( name_jvalue );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonModule::activateSemaphore( const String & _name )
    {
        for( const AndroidSemaphoreListenerDesc & desc : m_semaphoreListeners )
        {
            if( desc.name != _name.c_str() )
            {
                continue;
            }

            desc.cb.call_args( desc.args );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
