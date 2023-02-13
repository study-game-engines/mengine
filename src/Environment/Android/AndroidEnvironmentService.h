#pragma once

#include "Interface/AndroidEnvironmentServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/LoggerInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AndroidEnvironmentService
        : public ServiceBase<AndroidEnvironmentServiceInterface>
        , public AnalyticsEventProviderInterface
    {
    public:
        AndroidEnvironmentService();
        ~AndroidEnvironmentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        jclass getJClass( JNIEnv * _jenv, const Char * _signature ) const override;

    public:
        jmethodID getMengineActivityMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature ) const override;
        void callVoidMengineActivityMethod( JNIEnv * _jenv, jmethodID _method, ... ) const override;
        jobject callObjectMengineActivityMethod( JNIEnv * _jenv, jmethodID _method, ... ) const override;

    public:
        size_t getAndroidId( Char * _androidId, size_t _capacity ) const override;
        size_t getDeviceName( Char * _deviceName, size_t _capacity ) const override;
        size_t getDeviceLanguage( Char * _deviceName, size_t _capacity ) const override;
        size_t getAndroidPackageName( Char * _packageName, size_t _capacity ) const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;

    public:
        int32_t androidOpenAssetFile( const Char * _path ) override;
        int32_t androidAvailableAssetFile( int32_t _fileId ) override;
        int32_t androidReadAssetFile( int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer ) override;
        int32_t androidSkipAssetFile( int32_t _fileId, int32_t _offset ) override;
        void androidResetAssetFile( int32_t _fileId ) override;
        void androidCloseAssetFile( int32_t _fileId ) override;

    protected:
        void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;
        void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;

    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;

    protected:
        void notifyApplicationRun_();
        void notifyApplicationReady_();
        void notifyApplicationStop_();
        void notifyBootstrapperInitializeBaseServices_();
        void notifyBootstrapperCreateApplication_();

    protected:
        void exceptionCheck( JNIEnv * _jenv ) const;

    protected:
        AndroidEventationHubPtr m_androidEventationHub;

        LoggerInterfacePtr m_proxyLogger;
    };
}
