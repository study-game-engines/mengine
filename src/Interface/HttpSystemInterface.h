#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/String.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Params.h"

#include "Config/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef uint32_t HttpRequestID;
    //////////////////////////////////////////////////////////////////////////
    class HttpReceiverInterface
        : public Mixin
    {
    public:
        virtual void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpReceiverInterface> HttpReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class HttpSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "HttpSystem" )

    public:
        virtual HttpRequestID getMessage( const String & _url, const HttpReceiverInterfacePtr & _receiver ) = 0;
        virtual HttpRequestID postMessage( const String & _url, const MapParams & _params, const HttpReceiverInterfacePtr & _receiver ) = 0;
        virtual HttpRequestID headerData( const String & _url, const VectorString & _headers, const String & _data, const HttpReceiverInterfacePtr & _receiver ) = 0;

    public:
        virtual HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _category, const FilePath & _path, const HttpReceiverInterfacePtr & _receiver ) = 0;

    public:
        virtual bool cancelRequest( HttpRequestID _id ) = 0;
    };

#   define HTTP_SYSTEM()\
	SERVICE_GET(Mengine::HttpSystemInterface)
}
