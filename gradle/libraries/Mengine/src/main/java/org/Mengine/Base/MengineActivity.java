package org.Mengine.Base;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.Map;

import android.content.*;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.provider.Settings.Secure;
import android.view.KeyEvent;

public class MengineActivity extends SDLActivity {
    public static final String TAG = "MengineActivity";

    private boolean m_initializeBaseServices;

    private String m_androidId;

    private Map<Integer, InputStream> m_openFiles;
    private int m_fileEnumerator;

    private Map<String, Integer> m_requestCodes;

    class CallbackResponse {
        public Integer id;
        public MenginePlugin.CallbackInterface cb;
    };

    private ArrayList<CallbackResponse> m_callbackResponses;
    private int m_callbackResponseEnumerator;

    private static native void AndroidNativeMengine_setMengineAndroidActivityJNI(Object Activity);
    private static native void AndroidNativeMengine_quitMengineAndroidActivityJNI();
    private static native String AndroidNativeMengine_getCompanyName();
    private static native String AndroidNativeMengine_getProjectName();
    private static native int AndroidNativeMengine_getProjectVersion();
    private static native boolean AndroidNativeMengine_isDebugMode();
    private static native boolean AndroidNativeMengine_isDevelopmentMode();
    private static native boolean AndroidNativeMengine_isMasterRelease();
    private static native boolean AndroidNativeMengine_isBuildPublish();
    private static native String AndroidNativeMengine_getEngineGITSHA1();
    private static native String AndroidNativeMengine_getBuildTimestamp();
    private static native String AndroidNativeMengine_getBuildUsername();
    private static native String AndroidNativeMengine_getBuildVersion();
    private static native String AndroidNativeMengine_getConfigValue(String section, String key, String default_value);
    private static native boolean AndroidNativeMengine_getConfigValueBoolean(String section, String key, boolean default_value);
    private static native boolean AndroidNativeMengine_hasOption(String option);

    private static native void AndroidNativePython_addPlugin(String name, Object plugin);
    private static native void AndroidNativePython_call(String plugin, String method, int responseId, Object args[]);

    public MengineActivity() {
        m_initializeBaseServices = false;

        m_openFiles = new HashMap<Integer, InputStream>();
        m_fileEnumerator = 0;

        m_requestCodes = new HashMap<String, Integer>();

        m_callbackResponses = new ArrayList<CallbackResponse>();
        m_callbackResponseEnumerator = 0;
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "SDLApplication"
        };
    }

    public String getCompanyName() {
       return AndroidNativeMengine_getCompanyName();
    }

    public String getProjectName() {
        return AndroidNativeMengine_getProjectName();
    }

    public int getProjectVersion() {
        return AndroidNativeMengine_getProjectVersion();
    }

    public boolean isDebugMode() {
        return AndroidNativeMengine_isDebugMode();
    }

    public boolean isDevelopmentMode() {
        return AndroidNativeMengine_isDevelopmentMode();
    }

    public boolean isMasterRelease() {
        return AndroidNativeMengine_isMasterRelease();
    }

    public boolean isBuildPublish() {
        return AndroidNativeMengine_isBuildPublish();
    }

    public String getEngineGITSHA1() {
        return AndroidNativeMengine_getEngineGITSHA1();
    }

    public String getBuildTimestamp() {
        return AndroidNativeMengine_getBuildTimestamp();
    }

    public String getBuildUsername() {
        return AndroidNativeMengine_getBuildUsername();
    }

    public String getBuildVersion() {
        return AndroidNativeMengine_getBuildVersion();
    }

    public String getConfigValue(String section, String key, String default_value) {
        return AndroidNativeMengine_getConfigValue(section, key, default_value);
    }

    public boolean getConfigValueBoolean(String section, String key, boolean default_value) {
        return AndroidNativeMengine_getConfigValueBoolean(section, key, default_value);
    }

    public boolean hasOption(String option) {
        return AndroidNativeMengine_hasOption(option);
    }

    protected ArrayList<MenginePlugin> getPlugins() {
        MengineApplication app = (MengineApplication)this.getApplication();

        ArrayList<MenginePlugin> plugins = app.getPlugins();

        return plugins;
    }

    public <T> T findPlugin(Class<T> cls) {
        String name;

        try {
            Field PLUGIN_NAME = cls.getField("PLUGIN_NAME");

            name = (String)PLUGIN_NAME.get(null);
        } catch (NoSuchFieldException ex) {
            Log.e(TAG, "MengineActivity plugin not found field PLUGIN_NAME: " + cls.getName());

            return null;
        } catch (IllegalAccessException ex) {
            Log.e(TAG, "MengineActivity plugin invalid field PLUGIN_NAME: " + cls.getName());

            return null;
        }

        MengineApplication app = (MengineApplication)this.getApplication();

        MenginePlugin plugin = app.findPlugin(name);

        return (T)plugin;
    }

    public void sendEvent(String id, Object ... args) {
        for(MenginePlugin p : this.getPlugins()) {
            p.onEvent(this, id, args);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.i(TAG, "onCreate");

        if(mBrokenLibraries == true) {
            Log.e(TAG, "onCreate: broken libraries");

            AndroidNativeMengine_quitMengineAndroidActivityJNI();

            return;
        }

        AndroidNativeMengine_setMengineAndroidActivityJNI(this);

        Context context = this.getContext();
        ContentResolver resolver = context.getContentResolver();
        m_androidId = Secure.getString(resolver, Secure.ANDROID_ID);

        for(MenginePlugin p : this.getPlugins()) {
            p.setActivity(this);

            p.onCreate(this, savedInstanceState);
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onExtension(this);
        }
    }

    public void quitMengineApplication() {
        AndroidNativeMengine_quitMengineAndroidActivityJNI();
    }

    public void onMengineInitializeBaseServices() {
        m_initializeBaseServices = true;

        MengineLog.onMengineInitializeBaseServices();

        MengineLog.logInfo(TAG, "onMengineInitializeBaseServices");

        for(MenginePlugin p : this.getPlugins()) {
            p.onMengineInitializeBaseServices(this);
        }

        MengineApplication app = (MengineApplication)this.getApplication();
        app.onMengineInitializeBaseServices(this);
    }

    public void onMengineCreateApplication() {
        MengineLog.logInfo(TAG, "onMengineCreateApplication");

        for(MenginePlugin p : this.getPlugins()) {
            p.onMengineCreateApplication(this);
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onExtensionRun(this);
        }

        MengineApplication app = (MengineApplication)this.getApplication();
        app.onMengineCreateApplication(this);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        MengineLog.logInfo(TAG, "onActivityResult");

        for(MenginePlugin p : this.getPlugins()) {
            p.onActivityResult(this, requestCode, resultCode, data);
        }
    }

    @Override
    public void onStart(){
        super.onStart();

        MengineLog.logInfo(TAG, "onStart");

        for(MenginePlugin p : this.getPlugins()) {
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

        for(MenginePlugin p : this.getPlugins()) {
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

        for(MenginePlugin p : this.getPlugins()) {
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

        for(MenginePlugin p : this.getPlugins()) {
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

        for(MenginePlugin p : this.getPlugins()) {
            p.onNewIntent(this, intent);
        }
    }

    @Override
    public void onDestroy(){
        super.onDestroy();

        MengineLog.logInfo(TAG, "onDestroy");

        for(MenginePlugin p : this.getPlugins()) {
            p.onDestroy(this);
        }
    }

    @Override
    public void onRestart(){
        super.onRestart();

        MengineLog.logInfo(TAG, "onRestart");

        for(MenginePlugin p : this.getPlugins()) {
            p.onRestart(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onRestart");
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig){
        super.onConfigurationChanged(newConfig);

        for(MenginePlugin p : this.getPlugins()) {
            p.onConfigurationChanged(this, newConfig);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults){
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        for(MenginePlugin p : this.getPlugins()) {
            p.onRequestPermissionsResult(this, requestCode, permissions, grantResults);
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event){
        MengineLog.logInfo(TAG, "dispatchKeyEvent() action: " + event.getAction() + " code: " + event.getKeyCode());

        for(MenginePlugin p : this.getPlugins()) {
            if (p.dispatchKeyEvent(this, event) == true) {
                return true;
            }
        }

        return super.dispatchKeyEvent(event);
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        return new MengineSurface(context);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Kernel Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getAndroidId() {


        return m_androidId;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getDeviceName() {
        String deviceName = android.os.Build.MODEL;

        return deviceName;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public int genRequestCode(String name) {
        if (m_requestCodes.containsKey(name) == false) {
            int new_code = m_requestCodes.size();

            m_requestCodes.put(name, new_code);

            return new_code;
        }

        int code = m_requestCodes.get(name);

        return code;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Python Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonInitializePlugins() {
        AndroidNativePython_addPlugin("Activity", this);

        for(MenginePlugin p : this.getPlugins()) {
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
    ////////////////////////////////////////////////////////////////////////////////////////////////
    private static void appendBuildArgsString(StringBuilder py_args, String s) {
        py_args.append("\"");
        py_args.append(s.replaceAll("\"","\\\\\""));
        py_args.append("\"");
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonCall(String plugin, String method, Object ... args) {
        if (BuildConfig.DEBUG) {
            MengineLog.logInfo(TAG, "pythonCall [" + plugin + "] method [" + method + "] args [" + args + "]");
        }

        AndroidNativePython_call(plugin, method, 0, args);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonCallCb(String plugin, String method, MenginePlugin.CallbackInterface cb, Object ... args) {
        m_callbackResponseEnumerator++;

        int id = m_callbackResponseEnumerator;

        CallbackResponse cr = new CallbackResponse();
        cr.id = id;
        cr.cb = cb;

        m_callbackResponses.add(cr);

        MengineLog.logInfo(TAG, "pythonCall [" + plugin + "] method [" + method + "] response [" + id + "] args [" + args + "]");

        AndroidNativePython_call(plugin, method, id, args);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void responseCall(int id, Object result) {
        MengineLog.logInfo(TAG, "responseCall [" + id + "] result [" + result.toString() + "]");

        Iterator itr = m_callbackResponses.iterator();

        while(itr.hasNext())
        {
            CallbackResponse cr = (CallbackResponse)itr.next();

            if( cr.id != id )
            {
                continue;
            }

            cr.cb.callback(result);

            itr.remove();

            return;
        }

        MengineLog.logError(TAG, "responceCall [" + id + "] not found");
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void addPythonPlugin(String name, Object plugin) {
        MengineLog.logInfo(TAG, "addPythonPlugin [" + name + "] plugin: " + plugin.toString());

        AndroidNativePython_addPlugin(name, plugin);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Linking Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean linkingOpenURL(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(url));
        this.startActivity(Intent.createChooser(intent, ""));

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean linkingOpenMail(String email, String subject, String body) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(email));
        this.startActivity(Intent.createChooser(intent, ""));
        intent.putExtra(Intent.EXTRA_SUBJECT, subject);
        intent.putExtra(Intent.EXTRA_TEXT, body);
        this.startActivity(Intent.createChooser(intent, ""));

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Asset Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public int openAssetFile(String path) {
        try {
            InputStream stream = getAssets().open(path);

            m_fileEnumerator++;
            int id = m_fileEnumerator;

            m_openFiles.put(id, stream);

            return id;
        } catch(IOException ex) {
            MengineLog.logError(TAG, "open asset file [" + path + "] ex: " + ex);

            return 0;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public int availableAssetFile(int id) {
        InputStream stream = m_openFiles.get(id);

        int size = 0;

        try {
            size = stream.available();
        } catch (IOException ex) {
            MengineLog.logError(TAG, "available asset file [" + id + "] ex: " + ex);

            return 0;
        }

        return size;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public byte[] readAssetFile(int id, int offset, int size) {
        InputStream stream = m_openFiles.get(id);

        byte[] buffer = new byte[size];

        int read = 0;

        try {
            read = stream.read(buffer, offset, size);
        } catch (IOException ex) {
            MengineLog.logError(TAG, "read asset file [" + id + "] offset [" + offset + "] size [" + size + "] ex: " + ex);

            return null;
        }

        return buffer;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public long skipAssetFile(int id, long offset) {
        InputStream stream = m_openFiles.get(id);

        long skip;

        try {
            skip = stream.skip(offset);
        } catch (IOException ex) {
            MengineLog.logError(TAG, "skip asset file [" + id + "] offset [" + offset + "] ex: " + ex);

            return 0;
        }

        return skip;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void resetAssetFile(int id) {
        InputStream stream = m_openFiles.get(id);

        try {
            stream.reset();
        } catch (IOException ex) {
            MengineLog.logError(TAG, "reset asset file [" + id + "] ex: " + ex);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void closeAssetFile(int id) {
        InputStream stream = m_openFiles.get(id);

        try {
            stream.close();
        } catch (IOException ex) {
            MengineLog.logError(TAG, "close asset file [" + id + "] ex: " + ex);
        }

        m_openFiles.remove(id);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
}
