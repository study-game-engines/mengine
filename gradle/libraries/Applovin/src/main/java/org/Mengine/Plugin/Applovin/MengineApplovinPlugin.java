package org.Mengine.Plugin.Applovin;

import android.content.Context;
import android.os.Bundle;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.ThreadUtil;

import java.util.concurrent.TimeUnit;

public class MengineApplovinPlugin extends MenginePlugin {
    /**
     * - onApplovinPluginOnSdkInitialized - вызов после успешной инициализации (после можно загружать рекламу)
     * <p>
     * Interstitial
     * void initInterstitial(String interstitial_ad_unit_id)
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
     * void initRewarded(String rewarded_ad_unit_id)
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
     */

    private MaxInterstitialAd m_interstitialAd;
    private int m_retryAttemptInterstitial;

    private MaxRewardedAd m_rewardedAd;
    private int m_retryAttemptRewarded;

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("Applovin");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        final Context context = activity.getBaseContext();

        AppLovinSdk.getInstance(context).setMediationProvider("max");
        AppLovinSdk.initializeSdk(context, new AppLovinSdk.SdkInitializationListener() {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration) {
                MengineApplovinPlugin.this.logInfo("AppLovinSdk initialized: consent dialog [%s] country [%s]"
                    , configuration.getConsentDialogState().toString()
                    , configuration.getCountryCode()
                );

                //ToDo
                //MengineApplovinPlugin.this.pythonCall("onApplovinPluginOnSdkInitialized");
            }
        });
    }

    public void initInterstitial(String interstitial_ad_unit_id) {
        m_interstitialAd = new MaxInterstitialAd(interstitial_ad_unit_id, getActivity());

        MaxAdListener maxAdListener = new MaxAdListener() {
            @Override
            public void onAdLoaded(MaxAd ad) {
                m_retryAttemptInterstitial = 0;

                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                m_interstitialAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                m_retryAttemptInterstitial++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptInterstitial)));

                ThreadUtil.performOnMainThread(() -> {
                    m_interstitialAd.loadAd();
                }, delayMillis);

                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                m_interstitialAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onApplovinInterstitialOnAdDisplayFailed");
            }
        };

        m_interstitialAd.setListener(maxAdListener);
    }

    public void initRewarded(String rewarded_ad_unit_id) {
        m_rewardedAd = MaxRewardedAd.getInstance(rewarded_ad_unit_id, getActivity());

        MaxRewardedAdListener maxRewardedAdListener = new MaxRewardedAdListener() {
            @Override
            public void onRewardedVideoStarted(MaxAd ad) {
                m_retryAttemptRewarded = 0;

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoStarted");
            }

            @Override
            public void onRewardedVideoCompleted(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnRewardedVideoCompleted");
            }

            @Override
            public void onUserRewarded(MaxAd ad, MaxReward reward) {
                String label = reward.getLabel();
                int amount = reward.getAmount();

                MengineApplovinPlugin.this.logInfo("rewarded %s [%d]"
                    , label
                    , amount
                );

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnUserRewarded", label, amount);
            }

            @Override
            public void onAdLoaded(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoaded");
            }

            @Override
            public void onAdDisplayed(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayed");
            }

            @Override
            public void onAdHidden(MaxAd ad) {
                m_rewardedAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdHidden");
            }

            @Override
            public void onAdClicked(MaxAd ad) {
                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdClicked");
            }

            @Override
            public void onAdLoadFailed(String adUnitId, MaxError error) {
                m_retryAttemptRewarded++;

                long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptRewarded)));

                ThreadUtil.performOnMainThread(() -> {
                    m_rewardedAd.loadAd();
                }, delayMillis);

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdLoadFailed");
            }

            @Override
            public void onAdDisplayFailed(MaxAd ad, MaxError error) {
                m_rewardedAd.loadAd();

                MengineApplovinPlugin.this.pythonCall("onApplovinRewardedOnAdDisplayFailed");
            }
        };

        m_rewardedAd.setListener(maxRewardedAdListener);
    }

    public void loadInterstitial() {
        m_interstitialAd.loadAd();
    }

    public void loadRewarded() {
        m_rewardedAd.loadAd();
    }

    public void showInterstitial() {
        if (m_interstitialAd.isReady()) {
            m_interstitialAd.showAd();
        }
    }

    public void showRewarded() {
        if (m_rewardedAd.isReady()) {
            m_rewardedAd.showAd();
        }
    }
}
