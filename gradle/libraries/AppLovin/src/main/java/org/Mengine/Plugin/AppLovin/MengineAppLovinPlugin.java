package org.Mengine.Plugin.AppLovin;

import org.Mengine.Plugin.AppLovin.MengineAppLovinMediationInterface;

import org.Mengine.Base.MengineUtils;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineUtils;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdViewAdListener;
import com.applovin.mediation.MaxAdWaterfallInfo;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxNetworkResponseInfo;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;

import com.applovin.sdk.AppLovinAdSize;
import com.applovin.sdk.AppLovinPrivacySettings;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;
import com.applovin.sdk.AppLovinSdkUtils;

import android.content.Context;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.FrameLayout;
import androidx.annotation.NonNull;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class MengineAppLovinPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "AppLovin";
    public static boolean PLUGIN_EMBEDDING = true;

    /**
     * <p>
     * void initialize()
     * - onApplovinPluginOnSdkInitialized
     * <p>
     * установка Interstitial
     * void initInterstitial()
     * void loadInterstitial()
     * void showInterstitial()
     * - onApplovinInterstitialOnAdLoaded
     * - onApplovinInterstitialOnAdDisplayed
     * - onApplovinInterstitialOnAdHidden
     * - onApplovinInterstitialOnAdClicked
     * - onApplovinInterstitialOnAdLoadFailed
     * - onApplovinInterstitialOnAdDisplayFailed
     * <p>
     * установка Rewarded
     * void initRewarded()
     * void loadRewarded()
     * void showRewarded()
     * - onApplovinRewardedOnRewardedVideoStarted
     * - onApplovinRewardedOnRewardedVideoCompleted
     * - onApplovinRewardedOnUserRewarded
     * - onApplovinRewardedOnAdLoaded
     * - onApplovinRewardedOnAdDisplayed
     * - onApplovinRewardedOnAdHidden
     * - onApplovinRewardedOnAdClicked
     * - onApplovinRewardedOnAdLoadFailed
     * - onApplovinRewardedOnAdDisplayFailed
     * <p>
     * установка Banner
     * void initBanner()
     * void bannerVisible(boolean show)
     * - onApplovinBannerOnAdDisplayed
     * - onApplovinBannerOnAdHidden
     * - onApplovinBannerOnAdClicked
     * - onApplovinBannerOnAdLoadFailed
     * - onApplovinBannerOnAdDisplayFailed
     * - onApplovinBannerOnAdExpanded
     * - onApplovinBannerOnAdCollapsed
     * <p>
     * void showMediationDebugger()
     */

    enum ELoadAdStatus {
        ADLOAD_NONE,
        ADLOAD_AMAZON,
        ADLOAD_READY
    }

    private MaxInterstitialAd m_interstitialAd;
    private int m_retryAttemptInterstitial;

    private MaxRewardedAd m_rewardedAd;
    private int m_retryAttemptRewarded;

    private MaxAdView m_adView;

    private MengineAppLovinMediationInterface m_mediationAmazon;

    private List<MengineAppLovinAnalyticsInterface> m_analytics;

    @Override
    public void onEvent(MengineActivity activity, String id, Object ... args) {
        if(id == "ConsentStatus") {
            final Context context = activity.getBaseContext();

            Integer consent = (Integer)args[0];

            switch (consent) {
                case -1:
                case 0:
                case 1:
                    this.logInfo("setHasUserConsent: false");

                    AppLovinPrivacySettings.setHasUserConsent(false, context);
                    break;
                case 2:
                    this.logInfo("setHasUserConsent: true");

                    AppLovinPrivacySettings.setHasUserConsent(true, context);
                    break;
            }
        }
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        if (m_mediationAmazon != null) {
            m_mediationAmazon = null;
        }

        if (m_interstitialAd != null) {
            m_interstitialAd.destroy();
            m_interstitialAd = null;
        }

        if (m_rewardedAd != null) {
            m_rewardedAd.destroy();
            m_rewardedAd = null;
        }

        if (m_adView != null) {
            m_adView.destroy();
            m_adView = null;
        }
    }

    public void initialize() {
        MengineActivity activity = this.getActivity();
        final Context context = activity.getBaseContext();

        MengineAppLovinMediationInterface mediationAmazon = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinMediationAmazon", false);

        if (mediationAmazon != null) {
            try {
                mediationAmazon.initializeMediator(activity);

                m_mediationAmazon = mediationAmazon;
            } catch (Exception e) {
            }
        }

        m_analytics = new ArrayList<MengineAppLovinAnalyticsInterface>();

        MengineAppLovinAnalyticsInterface firebaseAnalytics = this.newInstance("org.Mengine.Plugin.AppLovin.MengineAppLovinFirebaseAnalytics", false);

        if (firebaseAnalytics != null) {
            if (firebaseAnalytics.initializeAnalytics(activity) == true) {
                m_analytics.add(firebaseAnalytics);
            }
        }

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);

        appLovinSdk.setMediationProvider("max");

        boolean OPTION_applovinverbose = activity.hasOption("applovinverbose");
        boolean AppLovin_VerboseLogging = activity.getConfigValueBoolean("AppLovinPlugin", "VerboseLogging", false);

        if (OPTION_applovinverbose == true || AppLovin_VerboseLogging == true) {
            this.logInfo("setVerboseLogging: true");

            appLovinSdk.getSettings().setVerboseLogging(true);
        }

        boolean AppLovin_IsAgeRestrictedUser = activity.getConfigValueBoolean("AppLovinPlugin", "IsAgeRestrictedUser", true);

        this.logInfo("setIsAgeRestrictedUser: %b"
            , AppLovin_IsAgeRestrictedUser
        );

        AppLovinPrivacySettings.setIsAgeRestrictedUser( AppLovin_IsAgeRestrictedUser, context );

        boolean AppLovin_CCPA = activity.getConfigValueBoolean("AppLovinPlugin", "CCPA", true);

        this.logInfo("setDoNotSell: %b"
                , AppLovin_IsAgeRestrictedUser
        );

        AppLovinPrivacySettings.setDoNotSell( AppLovin_CCPA, context );

        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineAppLovinPlugin.this.logInfo("AppLovinSdk initialized: country [%s]"
                    , configuration.getCountryCode()
                );

                MengineAppLovinPlugin.this.pythonCall("onApplovinPluginOnSdkInitialized");
            }
        });

        boolean OPTION_applovindebugger = activity.hasOption("applovindebugger");

        if (OPTION_applovindebugger == true) {
            this.showMediationDebugger();
        }
    }

    public void initBanner() throws Exception {
        MengineActivity activity = this.getActivity();

        String AppLovin_BannerAdUnitId = activity.getConfigValue("AppLovinPlugin", "BannerAdUnitId", "");

        if (AppLovin_BannerAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] BannerAdUnitId");
        }

        MaxAdView adView = new MaxAdView(AppLovin_BannerAdUnitId, activity);

        adView.setRequestListener(new MaxAdRequestListener() {
            @Override
            public void onAdRequestStarted(String adUnitId) {
                MengineAppLovinPlugin.this.logInfo("[Banner] onAdRequestStarted %s"
                        , adUnitId
                );

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdRequestStarted");
            }
        });

        adView.setListener(new MaxAdViewAdListener() {
            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd( "Banner","onAdLoaded", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdDisplayed", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdHidden", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdClicked", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Banner", "onAdLoadFailed", error);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Banner", "onAdDisplayFailed", error);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdDisplayFailed");
            }

            @Override
            public void onAdExpanded(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdExpanded", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdExpanded");
            }

            @Override
            public void onAdCollapsed(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdCollapsed", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdCollapsed");
            }
        });

        adView.setRevenueListener(new MaxAdRevenueListener() {
            @Override
            public void onAdRevenuePaid(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Banner", "onAdRevenuePaid", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinBannerOnAdRevenuePaid");

                for (MengineAppLovinAnalyticsInterface analytic : MengineAppLovinPlugin.this.m_analytics) {
                    analytic.eventRevenuePaid(ad);
                }
            }
        });

        int width = ViewGroup.LayoutParams.MATCH_PARENT;
        boolean tablet = AppLovinSdkUtils.isTablet(activity);
        AppLovinAdSize size = tablet ? AppLovinAdSize.LEADER : AppLovinAdSize.BANNER;
        int tablet_size_height = size.getHeight();

        int heightPx = AppLovinSdkUtils.dpToPx(activity, tablet_size_height);

        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(width, heightPx);
        adView.setLayoutParams(params);

        Window window = activity.getWindow();
        View view = window.getDecorView();
        int window_view_height = view.getHeight();

        adView.setTranslationY(window_view_height - heightPx);

        ViewGroup rootView = view.findViewById(android.R.id.content);
        rootView.addView(adView);

        if (m_mediationAmazon != null) {
            m_mediationAmazon.initializeMediatorBanner(activity, adView);
        } else {
            // Load the ad
            adView.loadAd();
        }

        adView.setVisibility(View.GONE);

        m_adView = adView;
    }

    public void bannerVisible(boolean show) {
        if (show) {
            m_adView.startAutoRefresh();
            m_adView.setVisibility(View.VISIBLE);
        } else {
            m_adView.stopAutoRefresh();
            m_adView.setVisibility(View.GONE);
        }
    }

    public void initInterstitial() throws Exception {
        MengineActivity activity = this.getActivity();

        String AppLovin_InterstitialAdUnitId = activity.getConfigValue("AppLovinPlugin", "InterstitialAdUnitId", "");

        if (AppLovin_InterstitialAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] InterstitialAdUnitId");
        }

        MaxInterstitialAd interstitialAd = new MaxInterstitialAd(AppLovin_InterstitialAdUnitId, activity);

        interstitialAd.setRequestListener(new MaxAdRequestListener() {
            @Override
            public void onAdRequestStarted(String adUnitId) {
                MengineAppLovinPlugin.this.logInfo("[Interstitial] onAdRequestStarted %s"
                        , adUnitId
                );

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdRequestStarted");
            }
        });

        interstitialAd.setListener(new MaxAdListener() {
            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Interstitial", "onAdLoaded", ad);

                m_retryAttemptInterstitial = 0;

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Interstitial", "onAdDisplayed", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Interstitial", "onAdHidden", ad);

                m_interstitialAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Interstitial", "onAdClicked", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Interstitial", "onAdLoadFailed", error);

                m_retryAttemptInterstitial++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptInterstitial)));

                MengineUtils.performOnMainThread(() -> {
                    m_interstitialAd.loadAd();
                }, delayMillis);

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Interstitial", "onAdDisplayFailed", error);

                m_interstitialAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayFailed");
            }
        });

        interstitialAd.setRevenueListener(new MaxAdRevenueListener() {
            @Override
            public void onAdRevenuePaid(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Interstitial", "onAdRevenuePaid", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinInterstitialOnAdRevenuePaid");

                for (MengineAppLovinAnalyticsInterface analytic : MengineAppLovinPlugin.this.m_analytics) {
                    analytic.eventRevenuePaid(ad);
                }
            }
        });

        m_interstitialAd = interstitialAd;
    }

    public void loadInterstitial() {
        MengineActivity activity = this.getActivity();

        if (m_mediationAmazon != null) {
            try {
                m_mediationAmazon.loadMediatorInterstitial(activity, m_interstitialAd);
            } catch (Exception e) {
            }
        } else {
            m_interstitialAd.loadAd();
        }
    }

    public void showInterstitial() {
        if (m_interstitialAd.isReady()) {
            m_interstitialAd.showAd();
        }
    }

    public void initRewarded() throws Exception {
        MengineActivity activity = this.getActivity();

        String AppLovin_RewardedAdUnitId = activity.getConfigValue("AppLovinPlugin", "RewardedAdUnitId", "");

        if (AppLovin_RewardedAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] RewardedAdUnitId");
        }

        MaxRewardedAd rewardedAd = MaxRewardedAd.getInstance(AppLovin_RewardedAdUnitId, activity);

        rewardedAd.setRequestListener(new MaxAdRequestListener() {
            @Override
            public void onAdRequestStarted(String adUnitId) {
                MengineAppLovinPlugin.this.logInfo("[Rewarded] onAdRequestStarted %s"
                        , adUnitId
                );

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdRequestStarted");
            }
        });

        rewardedAd.setListener(new MaxRewardedAdListener() {
            @Override
            public void onRewardedVideoStarted(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onRewardedVideoStarted", ad);

                m_retryAttemptRewarded = 0;

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoStarted");
            }

            @Override
            public void onRewardedVideoCompleted(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onRewardedVideoCompleted", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoCompleted");
            }

            @Override
            public void onUserRewarded(MaxAd ad, MaxReward reward) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onUserRewarded", ad);

                String label = reward.getLabel();
                int amount = reward.getAmount();

                MengineAppLovinPlugin.this.logInfo("rewarded %s [%d]"
                        , label
                        , amount
                );

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnUserRewarded", label, amount);
            }

            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onAdLoaded", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onAdDisplayed", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onAdHidden", ad);

                m_rewardedAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onAdClicked", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Rewarded", "onAdLoadFailed", error);

                m_retryAttemptRewarded++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptRewarded)));

                MengineUtils.performOnMainThread(() -> {
                    m_rewardedAd.loadAd();
                }, delayMillis);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                MengineAppLovinPlugin.this.logMaxError("Rewarded", "onAdDisplayFailed", error);

                m_rewardedAd.loadAd();

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayFailed");
            }
        });

        rewardedAd.setRevenueListener(new MaxAdRevenueListener() {
            @Override
            public void onAdRevenuePaid(MaxAd ad) {
                MengineAppLovinPlugin.this.logMaxAd("Rewarded", "onAdRevenuePaid", ad);

                MengineAppLovinPlugin.this.pythonCall("onApplovinRewardedOnAdRevenuePaid");

                for (MengineAppLovinAnalyticsInterface analytic : MengineAppLovinPlugin.this.m_analytics) {
                    analytic.eventRevenuePaid(ad);
                }
            }
        });

        m_rewardedAd = rewardedAd;
    }

    public void loadRewarded() {
        MengineActivity activity = this.getActivity();

        if (m_mediationAmazon != null) {
            try {
                m_mediationAmazon.loadMediatorRewarded(activity, m_rewardedAd);
            } catch (Exception e) {
            }
        } else {
            m_rewardedAd.loadAd();
        }
    }

    public void showRewarded() {
        if (m_rewardedAd.isReady() == true) {
            m_rewardedAd.showAd();
        }
    }

    public void logMaxAd(String type, String callback, MaxAd ad) {
        this.logInfo( "AppLovin: type: " + type + " callback: " + callback );
        this.logInfo( "Waterfall Name: " + ad.getWaterfall().getName() + " and Test Name: " + ad.getWaterfall().getTestName() );
        this.logInfo( "Waterfall latency was: " + ad.getWaterfall().getLatencyMillis() + " milliseconds" );

        for (MaxNetworkResponseInfo networkResponse : ad.getWaterfall().getNetworkResponses()) {
            String waterfallInfoStr = "Network -> " + networkResponse.getMediatedNetwork() +
                    "\n...adLoadState: " + networkResponse.getAdLoadState() +
                    "\n...latency: " + networkResponse.getLatencyMillis() + " milliseconds" +
                    "\n...credentials: " + networkResponse.getCredentials();

            if (networkResponse.getError() != null) {
                waterfallInfoStr += "\n...error: " + networkResponse.getError();
            }

            this.logInfo(waterfallInfoStr);
        }
    }

    public void logMaxError(String type, String callback, MaxError error) {
        this.logInfo( "AppLovin: type: " + type + " callback: " + callback );
        this.logInfo( "MaxError: code: " + error.getCode() + " message: " + error.getMessage() );
        this.logInfo( "MediatedNetwork: code" + error.getMediatedNetworkErrorCode() + " message: " + error.getMediatedNetworkErrorMessage() );

        MaxAdWaterfallInfo waterfall = error.getWaterfall();

        this.logMaxAdWaterfallInfo(waterfall);
    }

    public void logMaxAdWaterfallInfo(MaxAdWaterfallInfo waterfall) {
        this.logInfo( "Waterfall Name: " + waterfall.getName() + " and Test Name: " + waterfall.getTestName() );
        this.logInfo( "Waterfall latency was: " + waterfall.getLatencyMillis() + " milliseconds" );

        for (MaxNetworkResponseInfo networkResponse : waterfall.getNetworkResponses()) {
            this.logMaxNetworkResponseInfo(networkResponse);
        }
    }

    public void logMaxNetworkResponseInfo(MaxNetworkResponseInfo networkResponse) {
        String waterfallInfoStr = "Network -> " + networkResponse.getMediatedNetwork() +
                "\n...adLoadState: " + networkResponse.getAdLoadState() +
                "\n...latency: " + networkResponse.getLatencyMillis() + " milliseconds" +
                "\n...credentials: " + networkResponse.getCredentials();

        if (networkResponse.getError() != null) {
            waterfallInfoStr += "\n...error: " + networkResponse.getError();
        }

        this.logInfo(waterfallInfoStr);
    }

    public void showMediationDebugger() {
        MengineActivity activity = this.getActivity();
        final Context context = activity.getBaseContext();

        AppLovinSdk appLovinSdk = AppLovinSdk.getInstance(context);
        appLovinSdk.showMediationDebugger();
    }
}
