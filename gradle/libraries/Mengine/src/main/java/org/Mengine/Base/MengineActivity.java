package org.Mengine.Base;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Map;
import java.util.UUID;

import android.content.*;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.provider.Settings.Secure;
import android.view.KeyEvent;
import android.view.View;

public class MengineActivity extends SDLActivity {
    public static final String TAG = "MengineActivity";

    private static native void AndroidEnvironmentService_setMengineAndroidActivityJNI(Object activity);
    private static native void AndroidEnvironmentService_removeMengineAndroidActivityJNI();
    private static native void AndroidEnvironmentService_quitMengineAndroidActivityJNI();
    private static native String AndroidEnvironmentService_getCompanyName();
    private static native String AndroidEnvironmentService_getProjectName();
    private static native int AndroidEnvironmentService_getProjectVersion();
    private static native boolean AndroidEnvironmentService_isDebugMode();
    private static native boolean AndroidEnvironmentService_isDevelopmentMode();
    private static native boolean AndroidEnvironmentService_isMasterRelease();
    private static native boolean AndroidEnvironmentService_isBuildPublish();
    private static native String AndroidEnvironmentService_getEngineGITSHA1();
    private static native String AndroidEnvironmentService_getBuildTimestamp();
    private static native String AndroidEnvironmentService_getBuildUsername();
    private static native String AndroidEnvironmentService_getBuildVersion();

    private static native void AndroidNativePython_addPlugin(String name, Object plugin);
    private static native void AndroidNativePython_call(String plugin, String method, int responseId, Object args[]);
    private static native void AndroidNativePython_activateSemaphore(String name);

    private boolean m_initializeBaseServices;
    private boolean m_initializePython;
    private boolean m_destroy;

    private Map<Integer, InputStream> m_openFiles;
    private int m_fileEnumerator;

    private Map<String, MengineSemaphore> m_semaphores;

    private Map<String, Integer> m_requestCodes;

    class CallbackResponse {
        public Integer id;
        public MengineCallbackInterface cb;
    };

    private ArrayList<CallbackResponse> m_callbackResponses;
    private int m_callbackResponseEnumerator;

    public MengineActivity() {
        m_initializeBaseServices = false;
        m_initializePython = false;
        m_destroy = false;

        m_openFiles = new HashMap<>();
        m_fileEnumerator = 0;

        m_semaphores = new HashMap<>();

        m_requestCodes = new HashMap<>();

        m_callbackResponses = new ArrayList<>();
        m_callbackResponseEnumerator = 0;
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
            "SDL2"
        };
    }

    @Override
    protected String getMainSharedObject() {
        String mainSharedObject = getContext().getApplicationInfo().nativeLibraryDir + "/" + "libSDLApplication.so";

        return mainSharedObject;
    }

    public String getCompanyName() {
       return AndroidEnvironmentService_getCompanyName();
    }

    public String getProjectName() {
        return AndroidEnvironmentService_getProjectName();
    }

    public int getProjectVersion() {
        return AndroidEnvironmentService_getProjectVersion();
    }

    public boolean isDebugMode() {
        return AndroidEnvironmentService_isDebugMode();
    }

    public boolean isDevelopmentMode() {
        return AndroidEnvironmentService_isDevelopmentMode();
    }

    public boolean isMasterRelease() {
        return AndroidEnvironmentService_isMasterRelease();
    }

    public boolean isBuildPublish() {
        return AndroidEnvironmentService_isBuildPublish();
    }

    public String getEngineGITSHA1() {
        return AndroidEnvironmentService_getEngineGITSHA1();
    }

    public String getBuildTimestamp() {
        return AndroidEnvironmentService_getBuildTimestamp();
    }

    public String getBuildUsername() {
        return AndroidEnvironmentService_getBuildUsername();
    }

    public String getBuildVersion() {
        return AndroidEnvironmentService_getBuildVersion();
    }

    public MengineApplication getMengineApplication() {
        MengineApplication app = (MengineApplication)this.getApplication();

        return app;
    }

    public String getApplicationId() {
        MengineApplication app = this.getMengineApplication();

        String applicationId = app.getApplicationId();

        return applicationId;
    }

    protected ArrayList<MenginePlugin> getPlugins() {
        MengineApplication app = this.getMengineApplication();

        ArrayList<MenginePlugin> plugins = app.getPlugins();

        return plugins;
    }

    protected ArrayList<MenginePluginKeyListener> getKeyListeners() {
        MengineApplication app = this.getMengineApplication();

        ArrayList<MenginePluginKeyListener> listeners = app.getKeyListeners();

        return listeners;
    }

    @SuppressWarnings("unchecked")
    public <T> T getPlugin(Class<T> cls) {
        if (m_destroy == true) {
            return null;
        }

        MengineApplication app = this.getMengineApplication();

        T plugin = app.getPlugin(cls);

        return plugin;
    }

    public void sendEvent(MengineEvent event, Object ... args) {
        if (m_destroy == true) {
            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onEvent(this, event, args);
        }
    }

    public boolean hasMetaData(String name) {
        MengineApplication application = this.getMengineApplication();

        boolean result = application.hasMetaData(name);

        return result;
    }

    public String getMetaDataString(String name) {
        MengineApplication application = this.getMengineApplication();

        String value = application.getMetaDataString(name);

        return value;
    }

    public boolean getMetaDataBoolean(String name, boolean d) {
        MengineApplication application = this.getMengineApplication();

        boolean value = application.getMetaDataBoolean(name, d);

        return value;
    }

    public int getMetaDataInteger(String name, int d) {
        MengineApplication application = this.getMengineApplication();

        int value = application.getMetaDataInteger(name, d);

        return value;
    }

    public static View getContentView() {
        View view = SDLActivity.getContentView();

        return view;
    }

    public static Context getContext() {
        Context context = SDLActivity.getContext();

        return context;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        MengineLog.logInfo(TAG, "onCreate");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onCreate: broken libraries");

            return;
        }

        AndroidEnvironmentService_setMengineAndroidActivityJNI(this);

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.setActivity(this);

            try {
                p.onCreate(this, savedInstanceState);
            } catch (MenginePluginInvalidInitializeException e) {
                MengineLog.logError(TAG, "invalid onCreate plugin: %s"
                    , e.getPluginName()
                );

                this.finish();
            }
        }

        for (MenginePlugin p : plugins) {
            try {
                p.onExtensionInitialize(this);
            } catch (MenginePluginInvalidInitializeException e) {
                MengineLog.logError(TAG, "invalid onExtensionInitialize plugin: %s"
                    , e.getPluginName()
                );

                this.finish();
            }
        }
    }

    public void quitMengineApplication() {
        AndroidEnvironmentService_quitMengineAndroidActivityJNI();
    }

    public void onMengineInitializeBaseServices() {
        m_initializeBaseServices = true;

        MengineLog.onMengineInitializeBaseServices(this);

        MengineLog.logInfo(TAG, "onMengineInitializeBaseServices");

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onMengineInitializeBaseServices(this);
        }

        MengineApplication app = this.getMengineApplication();
        app.onMengineInitializeBaseServices(this);
    }

    public void onMengineCreateApplication() {
        MengineLog.logInfo(TAG, "onMengineCreateApplication");

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onMengineCreateApplication(this);
        }

        for (MenginePlugin p : plugins) {
            p.onExtensionRun(this);
        }

        MengineApplication app = this.getMengineApplication();
        app.onMengineCreateApplication(this);
    }

    public void onMengineApplicationRun() {
        MengineLog.logInfo(TAG, "onMengineApplicationRun");

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onMengineApplicationRun(this);
        }

        MengineApplication app = this.getMengineApplication();
        app.onMengineApplicationRun(this);
    }

    public void onMengineApplicationReady() {
        MengineLog.logInfo(TAG, "onMengineApplicationReady");

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onMengineApplicationReady(this);
        }

        MengineApplication app = this.getMengineApplication();
        app.onMengineApplicationReady(this);
    }

    public void onMengineApplicationStop() {
        MengineLog.logInfo(TAG, "onMengineApplicationStop");

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onMengineApplicationStop(this);
        }

        MengineApplication app = this.getMengineApplication();
        app.onMengineApplicationStop(this);

        MengineLog.onMengineApplicationStop(this);

        m_initializeBaseServices = false;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        MengineLog.logInfo(TAG, "onActivityResult");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onStart: broken libraries");

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onActivityResult(this, requestCode, resultCode, data);
        }
    }

    @Override
    public void onStart(){
        super.onStart();

        MengineLog.logInfo(TAG, "onStart");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onStart: broken libraries");

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onStart(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onStart");
        }
    }

    @Override
    protected void onStop() {
        super.onStop();

        MengineLog.logInfo(TAG, "onStop");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onStop: broken libraries");

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onStop(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onStop");
        }
    }

    @Override
    public void onPause() {
        super.onPause();

        MengineLog.logInfo(TAG, "onPause");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onPause: broken libraries");

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onPause(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onPause");
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        MengineLog.logInfo(TAG, "onResume");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onResume: broken libraries");

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onResume(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onResume");
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        MengineLog.logInfo(TAG, "onNewIntent");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onNewIntent: broken libraries");

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onNewIntent(this, intent);
        }
    }

    @Override
    public void onDestroy() {
        MengineLog.logInfo(TAG, "onDestroy");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onDestroy: broken libraries");

            super.onDestroy();

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onDestroy(this);
        }

        for (MenginePlugin p : plugins) {
            p.onExtensionFinalize(this);
        }

        for (MenginePlugin p : plugins) {
            p.setActivity(null);
        }

        m_openFiles = null;
        m_semaphores = null;
        m_requestCodes = null;
        m_callbackResponses = null;

        AndroidEnvironmentService_removeMengineAndroidActivityJNI();

        super.onDestroy();
    }

    @Override
    public void onRestart() {
        super.onRestart();

        MengineLog.logInfo(TAG, "onRestart");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onRestart: broken libraries");

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onRestart(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onRestart");
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        MengineLog.logInfo(TAG, "onConfigurationChanged: %s"
            , newConfig.toString()
        );

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onConfigurationChanged: broken libraries");

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onConfigurationChanged(this, newConfig);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        MengineLog.logInfo(TAG, "onRequestPermissionsResult");

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "onRequestPermissionsResult: broken libraries");

            return;
        }

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            p.onRequestPermissionsResult(this, requestCode, permissions, grantResults);
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        MengineLog.logInfo(TAG, "dispatchKeyEvent action: %d code: %d"
            , event.getAction()
            , event.getKeyCode()
        );

        if (mBrokenLibraries == true) {
            MengineLog.logError(TAG, "dispatchKeyEvent: broken libraries");

            return super.dispatchKeyEvent(event);
        }

        ArrayList<MenginePluginKeyListener> listeners = this.getKeyListeners();

        for (MenginePluginKeyListener l : listeners) {
            if (l.dispatchKeyEvent(this, event) == true) {
                return true;
            }
        }

        return super.dispatchKeyEvent(event);
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        return new MengineSurface(context);
    }

    /***********************************************************************************************
     * Kernel Methods
     **********************************************************************************************/

    public int genRequestCode(String name) {
        if (m_requestCodes.containsKey(name) == false) {
            int new_code = m_requestCodes.size();

            m_requestCodes.put(name, new_code);

            return new_code;
        }

        int code = m_requestCodes.get(name);

        return code;
    }

    /***********************************************************************************************
     * Python Methods
     **********************************************************************************************/

    public void pythonInitializePlugins() {
        MengineLog.logInfo(TAG, "python initialize");

        m_initializePython = true;

        AndroidNativePython_addPlugin("Activity", this);

        ArrayList<MenginePlugin> plugins = this.getPlugins();

        for (MenginePlugin p : plugins) {
            try {
                Class<?> cls = p.getClass();

                Field PLUGIN_EMBEDDING = cls.getField("PLUGIN_EMBEDDING");

                if( PLUGIN_EMBEDDING.getBoolean(null) == false ) {
                    continue;
                }
            } catch (NoSuchFieldException ex) {
                continue;
            } catch (IllegalAccessException ex) {
                continue;
            }

            String name = p.getPluginName();

            this.addPythonPlugin(name, p);
        }
    }

    public void pythonFinalizePlugins() {
        m_initializePython = false;
    }

    public void pythonCall(String plugin, String method, Object ... args) {
        if (m_initializePython == false) {
            MengineLog.logError(TAG,"invalid python plugin [%s] method [%s] args [%s] call before embedding"
                , plugin
                , method
                , args
            );

            return;
        }

        if (BuildConfig.DEBUG) {
            MengineLog.logInfo(TAG, "pythonCall plugin [%s] method [%s] args [%s]"
                , plugin
                , method
                , args
            );
        }

        AndroidNativePython_call(plugin, method, 0, args);
    }

    protected int pythonCallResponseCreate(MengineCallbackInterface cb) {
        m_callbackResponseEnumerator++;

        int id = m_callbackResponseEnumerator;

        CallbackResponse cr = new CallbackResponse();
        cr.id = id;
        cr.cb = cb;

        m_callbackResponses.add(cr);

        return id;
    }

    public void pythonCallCb(String plugin, String method, MengineCallbackInterface cb, Object ... args) {
        if (m_initializePython == false) {
            MengineLog.logError(TAG, "invalid python plugin [%s] method [%s] args [%s] call before embedding"
                , plugin
                , method
                , args
            );

            return;
        }

        int id = this.pythonCallResponseCreate(cb);

        if (BuildConfig.DEBUG) {
            MengineLog.logInfo(TAG, "pythonCall plugin [%s] method [%s] response [%d] args [%s]"
                , plugin
                , method
                , id
                , args
            );
        }

        AndroidNativePython_call(plugin, method, id, args);
    }

    protected CallbackResponse pythonCallResponseGet(int id) {
        Iterator<CallbackResponse> itr = m_callbackResponses.iterator();

        CallbackResponse response;

        while(itr.hasNext() == true)
        {
            response = itr.next();

            if( response.id != id )
            {
                continue;
            }

            itr.remove();

            return response;
        }

        return null;
    }

    public void pythonCallResponse(int id, Object result) {
        if (BuildConfig.DEBUG) {
            MengineLog.logInfo(TAG, "pythonCallResponse [%d] result [%s]"
                , id
                , result
            );
        }

        CallbackResponse response = this.pythonCallResponseGet(id);

        if (response == null) {
            MengineLog.logError(TAG, "responceCall [%d] not found"
                , id
            );

            return;
        }

        response.cb.callback(result);
    }

    public void addPythonPlugin(String name, Object plugin) {
        if (m_destroy == true) {
            return;
        }

        MengineLog.logMessage(TAG, "addPythonPlugin [%s] plugin: %s"
            , name
            , plugin
        );

        AndroidNativePython_addPlugin(name, plugin);
    }

    /***********************************************************************************************
     * Semaphore Methods
     **********************************************************************************************/

    public void activateSemaphore(String name) {
        if (m_destroy == true) {
            return;
        }

        MengineLog.logMessage(TAG, "activate semaphore [%s]"
            , name
        );

        MengineSemaphore semaphore = m_semaphores.get(name);

        if (semaphore == null) {
            MengineSemaphore new_semaphore = new MengineSemaphore(true);

            m_semaphores.put(name, new_semaphore);

            return;
        }

        semaphore.activate();
    }

    public void waitSemaphore(String name, MengineCallbackInterface cb) {
        MengineSemaphore semaphore = m_semaphores.get(name);

        if (semaphore == null) {
            MengineSemaphore new_semaphore = new MengineSemaphore(false);

            new_semaphore.addListener(cb);

            m_semaphores.put(name, new_semaphore);

            return;
        }

        semaphore.addListener(cb);
    }

    public void waitAndroidSemaphore(String name) {
        MengineLog.logMessage(TAG, "wait android semaphore [%s]"
            , name
        );

        MengineCallbackInterface cb = (Object) -> {
            AndroidNativePython_activateSemaphore(name);
        };

        this.waitSemaphore(name, cb);
    }

    /***********************************************************************************************
     * Linking Methods
     **********************************************************************************************/

    public boolean linkingOpenURL(String url) {
        MengineLog.logMessage(TAG, "linking open url [%s]"
            , url
        );

        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(url));
        this.startActivity(Intent.createChooser(intent, ""));

        return true;
    }

    public boolean linkingOpenMail(String email, String subject, String body) {
        MengineLog.logMessage(TAG, "linking open mail [%s] subject [%s] body: %s"
            , email
            , subject
            , body
        );

        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(email));
        this.startActivity(Intent.createChooser(intent, ""));
        intent.putExtra(Intent.EXTRA_SUBJECT, subject);
        intent.putExtra(Intent.EXTRA_TEXT, body);
        this.startActivity(Intent.createChooser(intent, ""));

        return true;
    }

    /***********************************************************************************************
     * Asset Methods
     **********************************************************************************************/

    public int openAssetFile(String path) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logMessage(TAG, "openAssetFile path [%s]"
                , path
            );
        }

        try {
            InputStream stream = getAssets().open(path);

            m_fileEnumerator++;
            int id = m_fileEnumerator;

            m_openFiles.put(id, stream);

            return id;
        } catch(IOException e) {
            MengineLog.logError(TAG, "open asset file [%s] ex: %s"
                , path
                , e.getLocalizedMessage()
            );

            return 0;
        }
    }

    public int availableAssetFile(int id) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logMessage(TAG, "availableAssetFile id [%d]"
                , id
            );
        }

        InputStream stream = m_openFiles.get(id);

        int size = 0;

        try {
            size = stream.available();
        } catch (IOException e) {
            MengineLog.logError(TAG, "available asset file [%d] ex: %s"
                , id
                , e.getLocalizedMessage()
            );

            return 0;
        }

        return size;
    }

    public byte[] readAssetFile(int id, int offset, int size) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logMessage(TAG, "readAssetFile id [%d] offset [%d] size [%d]"
                , id
                , offset
                , size
            );
        }

        InputStream stream = m_openFiles.get(id);

        byte[] buffer = new byte[size];

        int read = 0;

        try {
            read = stream.read(buffer, offset, size);
        } catch (IOException e) {
            MengineLog.logError(TAG, "read asset file [%d] offset [%d] size [%d] ex: %s"
                , id
                , offset
                , size
                , e.getLocalizedMessage()
            );

            return null;
        }

        return buffer;
    }

    public long skipAssetFile(int id, long offset) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logMessage(TAG, "skipAssetFile id [%d] offset [%d]"
                , id
                , offset
            );
        }

        InputStream stream = m_openFiles.get(id);

        long skip;

        try {
            skip = stream.skip(offset);
        } catch (IOException e) {
            MengineLog.logError(TAG, "skip asset file [%d] offset [%d] ex: %s"
                , id
                , offset
                , e.getLocalizedMessage()
            );

            return 0;
        }

        return skip;
    }

    public void resetAssetFile(int id) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logMessage(TAG, "resetAssetFile id [%d]"
                , id
            );
        }

        InputStream stream = m_openFiles.get(id);

        try {
            stream.reset();
        } catch (IOException e) {
            MengineLog.logError(TAG, "reset asset file [%d] ex: %s"
                , id
                , e.getLocalizedMessage()
            );
        }
    }

    public void closeAssetFile(int id) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logMessage(TAG, "closeAssetFile id [%d]"
                , id
            );
        }

        InputStream stream = m_openFiles.get(id);

        try {
            stream.close();
        } catch (IOException e) {
            MengineLog.logError(TAG, "close asset file [%d] ex: %s"
                , id
                , e.getLocalizedMessage()
            );
        }

        m_openFiles.remove(id);
    }
}
