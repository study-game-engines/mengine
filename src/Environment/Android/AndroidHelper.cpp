#include "AndroidHelper.h"

#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdIntTypes.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        jobject makeJObjectBoolean( JNIEnv * _jenv, bool _value )
        {
            jclass jclass_Boolean = _jenv->FindClass( "java/lang/Boolean" );

            jmethodID Boolean_constructor = _jenv->GetMethodID( jclass_Boolean, "<init>", "(Z)V" );

            MENGINE_ASSERTION( Boolean_constructor != nullptr, "invalid get android method 'Boolean <init> (Z)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Boolean, Boolean_constructor, _value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Boolean '%s'"
                , _value == true ? "true" : "false"
            );

            _jenv->DeleteLocalRef( jclass_Boolean );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject makeJObjectInteger( JNIEnv * _jenv, int32_t _value )
        {
            jclass jclass_Integer = _jenv->FindClass( "java/lang/Integer" );

            jmethodID Integer_constructor = _jenv->GetMethodID( jclass_Integer, "<init>", "(I)V" );

            MENGINE_ASSERTION( Integer_constructor != nullptr, "invalid get android method 'Integer <init> (I)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Integer, Integer_constructor, _value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Integer '%d'"
                , _value
            );

            _jenv->DeleteLocalRef( jclass_Integer );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject makeJObjectLong( JNIEnv * _jenv, int64_t _value )
        {
            jclass jclass_Long = _jenv->FindClass( "java/lang/Long" );

            jmethodID Long_constructor = _jenv->GetMethodID( jclass_Long, "<init>", "(J)V" );

            MENGINE_ASSERTION( Long_constructor != nullptr, "invalid get android method 'Long <init> (J)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Long, Long_constructor, _value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Long '%" MENGINE_PRId64 "'"
                , _value
            );

            _jenv->DeleteLocalRef( jclass_Long );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject makeJObjectFloat( JNIEnv * _jenv, float _value )
        {
            jclass jclass_Float = _jenv->FindClass( "java/lang/Float" );

            jmethodID Float_constructor = _jenv->GetMethodID( jclass_Float, "<init>", "(F)V" );

            MENGINE_ASSERTION( Float_constructor != nullptr, "invalid get android method 'Float <init> (F)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Float, Float_constructor, _value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Float '%f'"
                , _value
            );

            _jenv->DeleteLocalRef( jclass_Float );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject makeJObjectDouble( JNIEnv * _jenv, double _value )
        {
            jclass jclass_Double = _jenv->FindClass( "java/lang/Double" );

            jmethodID Double_constructor = _jenv->GetMethodID( jclass_Double, "<init>", "(D)V" );

            MENGINE_ASSERTION( Double_constructor != nullptr, "invalid get android method 'Double <init> (D)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_Double, Double_constructor, _value );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Double '%lf'"
                , _value
            );

            _jenv->DeleteLocalRef( jclass_Double );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject makeJObjectString( JNIEnv * _jenv, const Char * _value )
        {
            jclass jclass_String = _jenv->FindClass( "java/lang/String" );

            jmethodID String_constructor = _jenv->GetMethodID( jclass_String, "<init>", "(Ljava/lang/String;)V" );

            MENGINE_ASSERTION( String_constructor != nullptr, "invalid get android method 'String <init> (Ljava/lang/String;)V'" );

            jstring value_jstring = _jenv->NewStringUTF( _value );

            jobject value_jobject = _jenv->NewObject( jclass_String, String_constructor, value_jstring );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create String '%s'"
                , _value
            );

            _jenv->DeleteLocalRef( value_jstring );
            _jenv->DeleteLocalRef( jclass_String );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        jobject makeJObjectHashMap( JNIEnv * _jenv, int32_t _count )
        {
            jclass jclass_HashMap = _jenv->FindClass( "java/util/HashMap" );

            jmethodID HashMap_constructor = _jenv->GetMethodID( jclass_HashMap, "<init>", "(I)V" );

            MENGINE_ASSERTION( HashMap_constructor != nullptr, "invalid get android method 'HashMap <init> (I)V'" );

            jobject value_jobject = _jenv->NewObject( jclass_HashMap, HashMap_constructor, _count );

            MENGINE_ASSERTION( value_jobject != nullptr, "invalid create HashMap '%d'"
                , _count
            );

            _jenv->DeleteLocalRef( jclass_HashMap );

            return value_jobject;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString makeConstStringFromJString( JNIEnv * _jenv, jstring _value )
        {
            ConstString value_cstr;
            ANDROID_ENVIRONMENT_SERVICE()
                ->stringize( _jenv, _value, &value_cstr );

            return value_cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        String makeStringFromJString( JNIEnv * _jenv, jstring _value )
        {
            const Char * value_str = _jenv->GetStringUTFChars( _value, nullptr );

            String value_string( value_str );

            _jenv->ReleaseStringUTFChars( _value, value_str );

            return value_string;
        }
        //////////////////////////////////////////////////////////////////////////
        void jEnvExceptionCheck( JNIEnv * _jenv )
        {
            if( _jenv->ExceptionCheck() == JNI_FALSE )
            {
                return;
            }

            // Get our exception
            jthrowable jExcept = _jenv->ExceptionOccurred();

            // Clear the exception so we can call JNI again
            _jenv->ExceptionClear();

            // Get our exception message
            jclass jExceptClass = _jenv->GetObjectClass( jExcept );
            jmethodID jMessageMethod = _jenv->GetMethodID( jExceptClass, "getMessage", "()Ljava/lang/String;" );
            jstring jMessage = (jstring)( _jenv->CallObjectMethod( jExcept, jMessageMethod ) );
            const Char * pszMessage = _jenv->GetStringUTFChars( jMessage, NULL );

            // ...and log it.
            LOGGER_ERROR( "Java threw an exception: %s"
                , pszMessage
            );

            // Cleanup
            _jenv->ReleaseStringUTFChars( jMessage, pszMessage );
            _jenv->DeleteLocalRef( jMessage );
            _jenv->DeleteLocalRef( jExceptClass );
            _jenv->DeleteLocalRef( jExcept );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}