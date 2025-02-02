#pragma once

#include "Interface/Interface.h"
#include "Interface/ServantInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/String.h"
#include "Kernel/VectorString.h"

#define MENGINE_CURL_TIMEOUT_INFINITY (-1)
#define MENGINE_HTTP_REQUEST_INVALID (0)

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef uint32_t HttpRequestID;
    //////////////////////////////////////////////////////////////////////////
    typedef VectorString cURLHeaders;
    //////////////////////////////////////////////////////////////////////////
    struct cURLPostParam
    {
        String key;
        String value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<cURLPostParam> cURLPostParams;
    //////////////////////////////////////////////////////////////////////////
    class cURLResponseInterface
        : public Interface
    {
    public:
        virtual HttpRequestID getRequestId() const = 0;
        virtual uint32_t getStatus() const = 0;
        virtual const cURLHeaders & getHeaders() const = 0;
        virtual const String & getData() const = 0;
        virtual uint32_t getCode() const = 0;
        virtual const String & getError() const = 0;
        virtual bool isSuccessful() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLResponseInterface> cURLResponseInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class cURLReceiverInterface
        : public Interface
    {
    public:
        virtual void onHttpRequestComplete( const cURLResponseInterfacePtr & _response ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLReceiverInterface> cURLReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class cURLRequestListenerInterface
        : public Interface
    {
    public:
        virtual void onHttpRequest( HttpRequestID  _id, const String & _url ) = 0;
        virtual void onHttpResponse( const cURLResponseInterfacePtr & _response ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLRequestListenerInterface> cURLRequestListenerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class cURLServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "cURLService" );

    public:
        virtual HttpRequestID getMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc ) = 0;
        virtual HttpRequestID postMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc ) = 0;
        virtual HttpRequestID deleteMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc ) = 0;

    public:
        virtual HttpRequestID headerData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc ) = 0;

    public:
        virtual HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc ) = 0;

    public:
        virtual bool cancelRequest( HttpRequestID _id ) = 0;
        
    public:
        virtual int32_t addRequestListener( const cURLRequestListenerInterfacePtr & _listener, const DocumentPtr & _doc ) = 0;
        virtual void removeRequestListener( int32_t id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define CURL_SERVICE()\
    ((Mengine::cURLServiceInterface *)SERVICE_GET(Mengine::cURLServiceInterface))
//////////////////////////////////////////////////////////////////////////
