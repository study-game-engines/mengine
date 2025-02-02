#pragma once

#import "AppleAppLovinBaseDelegate.h"

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
#import "AppleAppLovinInterstitialAmazonLoader.h"
#endif

@interface AppleAppLovinInterstitialDelegate : AppleAppLovinBaseDelegate<MAAdRequestDelegate, MAAdDelegate, MAAdRevenueDelegate>

- (instancetype _Nonnull) initWithAdUnitIdentifier:(NSString * _Nonnull) adUnitIdentifier
                                      amazonSlotId:(NSString * _Nullable) amazonSlotId
                                  analyticsService:(AppleAppLovinAnalyticsService * _Nonnull) analytics;

- (BOOL) canYouShow;
- (BOOL) show;

- (void) loadAd;

@property (nonatomic, strong) MAInterstitialAd * _Nullable m_interstitialAd;
@property (nonatomic, assign) NSInteger m_retryAttempt;
@property (nonatomic, assign) NSInteger m_enumeratorRequest;
@property (nonatomic, assign) NSInteger m_requestId;

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON
@property (nonatomic, assign) AppleAppLovinInterstitialAmazonLoader * _Nullable m_amazonLoader;
#endif

@end
