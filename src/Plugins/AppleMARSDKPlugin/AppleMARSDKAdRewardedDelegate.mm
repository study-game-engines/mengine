#import "AppleMARSDKAdRewardedDelegate.h"

#include "Environment/Apple/AppleErrorHelper.h"
#include "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"

@implementation AppleMARSDKAdRewardedDelegate

#pragma mark -

@synthesize m_service;

- (instancetype _Nonnull)initWithService: (Mengine::AppleMARSDKServiceInterface* _Nonnull)service {
    self = [super init];
    
    self.m_service = service;
    
    return self;
}

#pragma mark - MARAdRewardedDelegate

- (void) MARAdRewardedDidFailed: (MARAdErrorCode)code withMessage: (NSString *)message adDict:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidFailed code: %d message: %s adDict: %s"
        , (int32_t)code
        , [message UTF8String]
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidFailed();
}

- (void) MARAdRewardedDidLoaded :(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidLoaded adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidLoaded();
}

- (void) MARAdRewardedDidShow :(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidShow adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidShow();
}

- (void) MARAdRewardedDidClicked :(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidClicked adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidClicked();
}

- (void) MARAdRewardedDidClosed :(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidClosed adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidClosed();
}

- (void) MARAdRewardedDidSkipped :(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidSkipped adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidSkipped();
}

- (void) MARAdRewardedDidFinished: (NSString *)itemName itemNum:(int)itemNum adDict:(NSDictionary *)adDict {
    LOGGER_MESSAGE( "MARAdRewardedDidFinished adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );
    
    const Mengine::AppleMARSDKProviderInterfacePtr & provider = m_service->getProvider();
    
    provider->onAdRewardedDidFinished([itemName UTF8String], itemNum);
}

@end
