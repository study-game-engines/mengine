package org.Mengine.Base;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineUtils;

import android.app.Application;
import android.content.Context;
import android.content.res.Configuration;
import android.util.Log;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MengineApplication extends Application {
    public static final String TAG = "MengineApplication";

    public ArrayList<MenginePlugin> m_plugins;
    public Map<String, MenginePlugin> m_dictionaryPlugins;

    public MengineApplication() {
        m_plugins = new ArrayList<MenginePlugin>();
        m_dictionaryPlugins = new HashMap<>();

        for (String n : this.getGradleAndroidPlugins()) {
            this.createPlugin(n);
        }
    }

    public String[] getGradleAndroidPlugins() {
        String[] empty = {};

        return empty;
    }

    public ArrayList<MenginePlugin> getPlugins() {
        return m_plugins;
    }

    public MenginePlugin findPlugin(String name) {
        MenginePlugin plugin = m_dictionaryPlugins.get(name);

        return plugin;
    }

    protected boolean createPlugin(String type) {
        ClassLoader cl = MengineActivity.class.getClassLoader();

        MenginePlugin plugin = MengineUtils.newInstance(TAG, type, true);

        if (plugin == null) {
            Log.e(TAG, "MengineApplication not found plugin: " + type);

            return false;
        }

        Class<?> cls = plugin.getClass();

        String name;

        try {
            Field PLUGIN_NAME = cls.getField("PLUGIN_NAME");

            name = (String)PLUGIN_NAME.get(null);
        } catch (NoSuchFieldException ex) {
            Log.e(TAG, "MengineApplication plugin not found field PLUGIN_NAME: " + type);

            return false;
        } catch (IllegalAccessException ex) {
            Log.e(TAG, "MengineApplication plugin invalid field PLUGIN_NAME: " + type);

            return false;
        }

        if (plugin.onInitialize(this, name) == false) {
            return false;
        }

        m_plugins.add(plugin);
        m_dictionaryPlugins.put(name, plugin);

        Log.i(TAG, "MengineApplication add plugin: " + type + " [" + name + "]");

        return true;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        Log.i(TAG, "onCreate");

        for (MenginePlugin p : this.m_plugins) {
            p.onAppCreate(this);
        }
    }

    @Override
    public void onTerminate() {
        super.onTerminate();

        Log.i(TAG, "onTerminate");

        for (MenginePlugin p : this.m_plugins) {
            p.onFinalize(this);
        }

        for (MenginePlugin p : this.m_plugins) {
            p.onAppTerminate(this);
        }
    }

    @Override
    public void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        Log.i(TAG, "attachBaseContext");

        for (MenginePlugin p : this.m_plugins) {
            p.onAppAttachBaseContext(this, base);
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        Log.i(TAG, "onConfigurationChanged");

        for (MenginePlugin p : this.m_plugins) {
            p.onAppConfigurationChanged(this, newConfig);
        }
    }
}