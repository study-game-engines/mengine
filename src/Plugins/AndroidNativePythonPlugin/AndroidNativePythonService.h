#pragma once

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidEventation.h"

#include "AndroidNativePythonInterface.h"
#include "AndroidNativePythonEventHandlerInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"
#include "Kernel/Pair.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class AndroidNativePythonService
        : public ServiceBase<AndroidNativePythonServiceInterface>
        , public AndroidNativePythonEventHandlerInterface
    {
    public:
        AndroidNativePythonService();
        ~AndroidNativePythonService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        typedef AndroidEventation<AndroidNativePythonEventHandlerInterface> PythonEventation;
        typedef IntrusivePtr<PythonEventation> PythonEventationPtr;
        typedef typename PythonEventation::LambdaEventHandler LambdaPythonEventHandler;

    public:
        void addCommand( const LambdaPythonEventHandler & _command );

    protected:
        PyObject * makePythonAttribute( JNIEnv * _jenv, jobject _obj );

    public:
        void pythonMethod( const String & _plugin, const String & _method, int32_t _id, jobjectArray _args ) override;
        void addPlugin( const String & _name, jobject _plugin ) override;
        void activateSemaphore( const String & _name ) override;

    public:
        void setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb ) override;
        bool androidResponse( JNIEnv * _jenv, int32_t _id, const pybind::object & _result ) const override;

    public:
        void androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        bool androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        int32_t androidIntegerMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        int64_t androidLongMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        float androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        double androidDoubleMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        String androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        PyObject * androidConfigMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;

    public:
        void waitAndroidSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args ) override;

    protected:
        bool getAndroidMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jfree, uint32_t * const _freeCount, jobject * const _jplugin, jmethodID * const _jmethodId ) const;

    protected:
        pybind::kernel_interface * m_kernel;

        typedef Map<Pair<ConstString, ConstString>, pybind::object> MapAndroidCallbacks;
        MapAndroidCallbacks m_callbacks;

        struct AndroidSemaphoreListenerDesc
        {
            ConstString name;
            pybind::object cb;
            pybind::args args;
        };

        typedef Vector<AndroidSemaphoreListenerDesc> VectorAndroidSemaphoreListeners;
        VectorAndroidSemaphoreListeners m_semaphoreListeners;

        typedef Map<ConstString, jobject> MapAndroidPlugins;
        MapAndroidPlugins m_plugins;

        PythonEventationPtr m_eventation;
    };
}
