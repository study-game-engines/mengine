#pragma once

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
#import "AppleAppLovinInterstitialAmazonLoader.h"
#endif

@interface AppleAppLovinInterstitialDelegate : NSObject<MAAdDelegate, MAAdRevenueDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitIdentifier
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId;

- (BOOL) hasLoaded;
- (BOOL) show;

@property (nonatomic, strong) MAInterstitialAd * _Nullable m_interstitialAd;
@property (nonatomic, assign) NSInteger m_retryAttempt;

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
@property (nonatomic, assign) AppleAppLovinInterstitialAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
