package org.Mengine.Plugin.DevToDev;

import android.os.Bundle;
import androidx.annotation.NonNull;

import com.devtodev.analytics.external.DTDLogLevel;
import com.devtodev.analytics.external.analytics.DTDAccrualType;
import com.devtodev.analytics.external.analytics.DTDAnalytics;
import com.devtodev.analytics.external.analytics.DTDAnalyticsConfiguration;
import com.devtodev.analytics.external.analytics.DTDCustomEventParameters;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import java.util.Map;

public class MengineDevToDevPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "DevToDev";
    public static boolean PLUGIN_EMBEDDING = true;

    public boolean m_initializeSuccessful = false;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        DTDAnalytics.INSTANCE.setInitializationCompleteCallback(() -> {
            MengineDevToDevPlugin.this.logInfo("Initialized has been finished");

            m_initializeSuccessful = true;

            return null;
        });

        DTDAnalytics.INSTANCE.coppaControlEnable();

        DTDAnalyticsConfiguration configuration = new DTDAnalyticsConfiguration();

        if (BuildConfig.DEBUG) {
            configuration.setLogLevel(DTDLogLevel.Debug);
        } else {
            configuration.setLogLevel(DTDLogLevel.Error);
        }

        String appKey = activity.getString(R.string.dev_to_dev_app_id);

        DTDAnalytics.INSTANCE.initialize(appKey, configuration, activity);
    }

    public void tutorialEvent(int stateOrStep) {
        if( m_initializeSuccessful == false ) {
            return;
        }

        DTDAnalytics.INSTANCE.tutorial(stateOrStep);
    }

    public void setCurrentLevelEvent(int level) {
        if( m_initializeSuccessful == false ) {
            return;
        }

        DTDAnalytics.INSTANCE.setCurrentLevel(level);
    }

    public void levelUpEvent(int level) {
        if( m_initializeSuccessful == false ) {
            return;
        }

        DTDAnalytics.INSTANCE.levelUp(level);
    }

    public void currencyAccrualEvent(String currencyName, int currencyAmount, String sourceName, int accrualType) {
        if( m_initializeSuccessful == false ) {
            return;
        }

        DTDAnalytics.INSTANCE.currencyAccrual(currencyName, currencyAmount, sourceName, DTDAccrualType.values()[accrualType]);
    }

    public void realCurrencyPaymentEvent(String paymentId, float inAppPrice, String inAppName, String inAppCurrencyISOCode) {
        if( m_initializeSuccessful == false ) {
            return;
        }

        DTDAnalytics.INSTANCE.realCurrencyPayment(paymentId, inAppPrice, inAppName, inAppCurrencyISOCode);
    }

    public void virtualCurrencyPaymentEvent(String purchaseId, String purchaseType, int purchaseAmount, int purchasePrice, String purchaseCurrency) {
        if( m_initializeSuccessful == false ) {
            return;
        }

        DTDAnalytics.INSTANCE.virtualCurrencyPayment(purchaseId, purchaseType, purchaseAmount, purchasePrice, purchaseCurrency);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineActivity activity, String eventName, long timestamp, Map<String, Object> parameters) {
        if( m_initializeSuccessful == false ) {
            return;
        }

        DTDCustomEventParameters params = new DTDCustomEventParameters();
        
        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
            String name = entry.getKey();
            Object parameter = entry.getValue();
            if (parameter instanceof Boolean) {
                params.add(name, (Boolean)parameter);
            } else if (parameter instanceof Long) {
                params.add(name, (Long)parameter);
            } else if (parameter instanceof Double) {
                params.add(name, (Double)parameter);
            } else if (parameter instanceof String) {
                params.add(name, (String)parameter);
            } else {
                this.logError("onMengineAnalyticsEvent: unsupported parameter [%s] %s"
                        , parameter.getClass().toString()
                        , parameter.toString()
                );

                return;
            }
        }

        this.logEvent(eventName, params);
    }
    
    public void logEvent(@NonNull String eventName, @NonNull DTDCustomEventParameters params) {
        if( m_initializeSuccessful == false ) {
            return;
        }

        DTDAnalytics.INSTANCE.customEvent(eventName, params);
    }
}
