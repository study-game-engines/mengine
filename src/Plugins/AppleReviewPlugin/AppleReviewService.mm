#include "AppleReviewService.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

#import <Foundation/Foundation.h>

#import <StoreKit/StoreKit.h>

////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleReviewService, Mengine::AppleReviewService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleReviewService::AppleReviewService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleReviewService::~AppleReviewService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleReviewService::launchTheInAppReview()
    {
        if (@available(iOS 14.0, *)) {
            UIWindow * appWindow = [UIApplication sharedApplication].delegate.window;
            [SKStoreReviewController requestReviewInScene:appWindow.windowScene];
        } else if (@available(iOS 10.3, *)) {
            [SKStoreReviewController requestReview];
        }
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleReviewService::_initializeService()
    {
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleReviewService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
