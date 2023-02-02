package org.Mengine.Base;

import android.app.Activity;
import android.content.Intent;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;

import androidx.annotation.NonNull;
import androidx.lifecycle.LifecycleOwner;

import java.util.ArrayList;
import java.util.Map;

public class MenginePlugin {
    private MengineApplication m_application;
    private MengineActivity m_activity;
    private ArrayList<MenginePluginExtension> m_extensions;
    private String m_pluginName;

    public static int LM_SILENT = 0;
    public static int LM_FATAL = 1;
    public static int LM_CRITICAL = 2;
    public static int LM_MESSAGE_RELEASE = 3;
    public static int LM_ERROR = 4;
    public static int LM_PERFOMANCE = 5;
    public static int LM_STATISTIC = 6;
    public static int LM_WARNING = 7;
    public static int LM_MESSAGE = 8;
    public static int LM_INFO = 9;
    public static int LM_DEBUG = 10;
    public static int LM_VERBOSE = 11;

    public MengineApplication getApplication() {
        return m_application;
    }

    public Context getApplicationContext() {
        Context context = m_application.getApplicationContext();

        return context;
    }

    public MengineActivity getActivity() {
        return m_activity;
    }

    public boolean onInitialize(MengineApplication application, String pluginName) {
        m_application = application;

        m_pluginName = pluginName;

        m_extensions = new ArrayList<>();

        return true;
    }

    public void onFinalize(MengineApplication application) {
        m_application = null;
    }

    public void setActivity(MengineActivity activity) {
        m_activity = activity;
    }

    public String getPluginName() {
        return m_pluginName;
    }

    public <T> T newInstance( String name, boolean exist ) {
        T instance = MengineUtils.newInstance(m_pluginName, name, exist);

        return instance;
    }

    public void logInfo(String format, Object ... args) {
        MengineLog.logInfo(m_pluginName, format, args);
    }

    public void logWarning(String format, Object ... args) {
        MengineLog.logWarning(m_pluginName, format, args);
    }

    public void logError(String format, Object ... args) {
        MengineLog.logError(m_pluginName, format, args);
    }

    public void pythonCall(String method, Object ... args) {
        if (m_activity == null) {
            return;
        }

        m_activity.pythonCall(m_pluginName, method, args);
    }

    public void pythonCallCb(String method, MengineCallbackInterface cb, Object ... args) {
        if (m_activity == null) {
            return;
        }

        m_activity.pythonCallCb(m_pluginName, method, cb, args);
    }

    public void activateSemaphore(String name) {
        if (m_activity == null) {
            return;
        }

        m_activity.activateSemaphore(name);
    }

    public void waitSemaphore(String name, MengineCallbackInterface cb) {
        if (m_activity == null) {
            return;
        }

        m_activity.waitSemaphore(name, cb);
    }

    public void onAppCreate(MengineApplication application) {
        //Empty
    }

    public void onAppTerminate(MengineApplication application) {
        //Empty
    }

    public void onAppAttachBaseContext(MengineApplication application, Context base) {
        //Empty
    }

    public void onAppConfigurationChanged(MengineApplication application, Configuration newConfig) {
        //Empty
    }

    public void onEvent(MengineActivity activity, String id, Object ... args) {
        //Empty
    }

    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        //Empty
    }

    public void addExtension(String type) {
        MenginePluginExtension extension = this.newInstance(type, false);

        if (extension == null) {
            return;
        }

        if( extension.onInitialize(m_activity, this) == false) {
            return;
        }

        m_extensions.add(extension);
    }

    public void onExtension(MengineActivity activity) {
        //Empty
    }

    public void onExtensionRun(MengineActivity activity) {
        for( MenginePluginExtension extension : m_extensions ) {
            extension.onRun(activity, this);
        }
    }

    public void onExtensionFinalize(MengineActivity activity) {
        for( MenginePluginExtension extension : m_extensions ) {
            extension.onFinalize(activity, this);
        }

        m_extensions = null;
    }

    public void onMengineInitializeBaseServices(MengineActivity activity) {
        //Empty
    }

    public void onMengineCreateApplication(MengineActivity activity) {
        //Empty
    }

    public void onMengineApplicationRun(MengineActivity activity) {
        //Empty
    }

    public void onMengineApplicationReady(MengineActivity activity) {
        //Empty
    }

    public void onMengineApplicationStop(MengineActivity activity) {
        //Empty
    }

    public void onMengineAnalyticsEvent(MengineActivity activity, String eventName, long timestamp, Map<String, Object> parameters) {
        //Empty
    }

    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        //Empty
    }

    public void onStart(MengineActivity activity) {
        //Empty
    }

    public void onStop(MengineActivity activity) {
        //Empty
    }

    public void onPause(MengineActivity activity) {
        //Empty
    }

    public void onResume(MengineActivity activity) {
        //Empty
    }

    public void onNewIntent(MengineActivity activity, Intent intent) {
        //Empty
    }

    public void onDestroy(MengineActivity activity) {
        //Empty
    }

    public void onRestart(MengineActivity activity) {
        //Empty
    }

    public void onConfigurationChanged(MengineActivity activity,Configuration newConfig) {
        //Empty
    }

    public void onRequestPermissionsResult(MengineActivity activity, int requestCode, String[] permissions, int[] grantResults) {
        //Empty
    }

    public boolean dispatchKeyEvent(MengineActivity activity, KeyEvent event) {
        //Empty

        return false;
    }

    public void onActivityLifecycleResumed(Activity activity) {
        //Empty
    }

    public void onActivityLifecyclePaused(Activity activity) {
        //Empty
    }

    public void onActivityLifecycleStarted(Activity activity) {
        //Empty
    }

    public void onActivityLifecycleStopped(Activity activity) {
        //Empty
    }

    public void onActivityLifecycleDestroyed(Activity activity) {
        //Empty
    }

    public void onActivityLifecycleCreated(Activity activity, Bundle bundle) {
        //Empty
    }

    public void onActivityLifecycleSaveInstanceState(Activity activity, Bundle bundle) {
        //Empty
    }

    public void onTrimMemory(MengineApplication application, int level) {
        //Empty
    }

    public void onConfigurationChanged(MengineApplication application, @NonNull Configuration newConfig) {
        //Empty
    }

    public void onLowMemory(MengineApplication application) {
        //Empty
    }

    public void onMengineLogger(String category, int level, int filter, int color, String msg) {
        //Empty
    }
}