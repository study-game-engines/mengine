#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleFacebookProviderInterface
        : public ServantInterface
    {
    public:
        virtual void onFacebookLoginSuccess( const Char * token ) = 0;
        virtual void onFacebookLoginCancel() = 0;

        virtual void onFacebookError( const Char * errorMessage ) = 0;

        virtual void onFacebookShareSuccess( const Char * postId ) = 0;
        virtual void onFacebookShareCancel() = 0;
        virtual void onFacebookShareError( const Char * errorMessage ) = 0;

        virtual void onFacebookProfilePictureLinkGet( const Char * userId, bool success, const Char * pictureURL ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleFacebookProviderInterface> AppleFacebookProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleFacebookServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFacebookService" )

    public:
        virtual void setProvider( const AppleFacebookProviderInterfacePtr & _provider ) = 0;
        virtual const AppleFacebookProviderInterfacePtr & getProvider() const = 0;

    public:
        virtual void login() = 0;
        virtual void logout() = 0;
        virtual bool isLoggedIn() const = 0;
        virtual const Char * getAccessToken() const = 0;
        virtual void shareLink( const Char * link, const Char * picture ) = 0;
        virtual void getProfilePictureLink() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FACEBOOK_SERVICE()\
    ((Mengine::AppleFacebookServiceInterface *)SERVICE_GET(Mengine::AppleFacebookServiceInterface))
//////////////////////////////////////////////////////////////////////////
