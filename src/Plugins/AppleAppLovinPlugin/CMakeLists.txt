MENGINE_PROJECT(AppleAppLovinPlugin)

ADD_FILTER(
src
    AppleAppLovinPlugin.h
    AppleAppLovinPlugin.mm

    AppleAppLovinInterface.h
    
    AppleAppLovinBaseDelegate.h
    AppleAppLovinBaseDelegate.mm

    AppleAppLovinInterstitialDelegate.h
    AppleAppLovinInterstitialDelegate.mm

    AppleAppLovinRewardedDelegate.h
    AppleAppLovinRewardedDelegate.mm

    AppleAppLovinBannerDelegate.h
    AppleAppLovinBannerDelegate.mm

    AppleAppLovinService.h
    AppleAppLovinService.mm

    AppleAppLovinAnalyticsDelegate.h

    AppleAppLovinAnalyticsService.h
    AppleAppLovinAnalyticsService.mm
)

if(MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS)
    ADD_FILTER(
    firebase
	AppleAppLovinFirebaseAnalytics.h
	AppleAppLovinFirebaseAnalytics.mm
    )
endif()

if(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_AMAZON)
    ADD_FILTER(
    amazon
        AppleAppLovinAmazonService.h
        AppleAppLovinAmazonService.mm
        
        AppleAppLovinInterstitialAmazonLoader.h
        AppleAppLovinInterstitialAmazonLoader.mm
        
        AppleAppLovinRewardedAmazonLoader.h
        AppleAppLovinRewardedAmazonLoader.mm
        
        AppleAppLovinBannerAmazonLoader.h
        AppleAppLovinBannerAmazonLoader.mm
    )
endif()

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleAppLovinScriptEmbedding.h
        AppleAppLovinScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_APPLOVIN)

ADD_MENGINE_COCOAPOD("AppLovinSDK" "NO-GIT" "11.7.1")

macro(ADD_APPLOVIN_MEDIATION MEDIATION POD VERSION)
    ADD_PLUGIN_OPTION(MENGINE_PLUGIN_APPLE_APPLOVIN MEDIATION_${MEDIATION} OFF "MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION}")
    if(MENGINE_PLUGIN_APPLE_APPLOVIN_MEDIATION_${MEDIATION})
        ADD_MENGINE_COCOAPOD(${POD} "NO-GIT" ${VERSION})
    endif()
endmacro()

ADD_APPLOVIN_MEDIATION(ADCOLONY "AppLovinMediationAdColonyAdapter" "4.9.0.0.3")
ADD_APPLOVIN_MEDIATION(AMAZON "AppLovinMediationAmazonAdMarketplaceAdapter" "4.5.6.4")
ADD_APPLOVIN_MEDIATION(AMAZON "AmazonPublisherServicesSDK" "4.5.6")
ADD_APPLOVIN_MEDIATION(BIDMACHINE "AppLovinMediationBidMachineAdapter" "2.0.0.5.0")
ADD_APPLOVIN_MEDIATION(CHARTBOOST "AppLovinMediationChartboostAdapter" "9.1.0.1")
ADD_APPLOVIN_MEDIATION(CRITEO "AppLovinMediationCriteoAdapter" "4.5.0.7")
ADD_APPLOVIN_MEDIATION(DTEXCHANGE "AppLovinMediationFyberAdapter" "8.1.9.0")
ADD_APPLOVIN_MEDIATION(ADMOB "AppLovinMediationGoogleAdapter" "9.14.0.2")
ADD_APPLOVIN_MEDIATION(HYPRMX "AppLovinMediationHyprMXAdapter" "6.0.3.1")
ADD_APPLOVIN_MEDIATION(INMOBI "AppLovinMediationInMobiAdapter" "10.1.2.6")
ADD_APPLOVIN_MEDIATION(IRONSOURCE "AppLovinMediationIronSourceAdapter" "7.2.7.0.1")
ADD_APPLOVIN_MEDIATION(LIFTOFF "AppLovinMediationLifftoffAdapter" "6.12.1.2")
ADD_APPLOVIN_MEDIATION(LINE "AppLovinMediationLineAdapter" "2.4.20220630.2")
ADD_APPLOVIN_MEDIATION(MAIO "AppLovinMediationMaioAdapter" "1.6.3.1")
ADD_APPLOVIN_MEDIATION(META "AppLovinMediationFacebookAdapter" "6.12.0.1")
ADD_APPLOVIN_MEDIATION(MINTEGRAL "AppLovinMediationMintegralAdapter" "7.2.9.0.1")
ADD_APPLOVIN_MEDIATION(MOBILEFUSE "AppLovinMediationMobileFuseAdapter" "1.4.0.0")
ADD_APPLOVIN_MEDIATION(MYTARGET "AppLovinMediationMyTargetAdapter" "5.16.0.2")
ADD_APPLOVIN_MEDIATION(NEND "AppLovinMediationNendAdapter" "7.4.0.0")
ADD_APPLOVIN_MEDIATION(OGURY "AppLovinMediationOguryPresageAdapter" "4.0.0.0")
ADD_APPLOVIN_MEDIATION(PANGLE "AppLovinMediationByteDanceAdapter" "4.8.1.0.2")
ADD_APPLOVIN_MEDIATION(SMAATO "AppLovinMediationSmaatoAdapter" "22.0.1.0")
ADD_APPLOVIN_MEDIATION(TAPJOY "AppLovinMediationTapjoyAdapter" "12.11.1.2")
ADD_APPLOVIN_MEDIATION(TENCENT "AppLovinMediationTencentGDTAdapter" "4.14.10.0")
ADD_APPLOVIN_MEDIATION(UNITYADS "AppLovinMediationUnityAdsAdapter" "4.5.0.2")
ADD_APPLOVIN_MEDIATION(VERVE "AppLovinMediationVerveAdapter" "2.17.0.0")
ADD_APPLOVIN_MEDIATION(YAHOO "AppLovinMediationVerizonAdsAdapter" "2.2.0.7")
ADD_APPLOVIN_MEDIATION(YANDEX "AppLovinMediationYandexAdapter" "5.5.0.0")

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
