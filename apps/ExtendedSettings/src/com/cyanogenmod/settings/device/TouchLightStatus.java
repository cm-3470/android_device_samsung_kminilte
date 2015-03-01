package com.cyanogenmod.settings.device;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceManager;

public class TouchLightStatus implements OnPreferenceChangeListener {

    private static final String FILE = "/sys/class/misc/notification/touchlight_enabled";

    /**
     * Checks if the feature is supported
     * @return True if the feature is supported, otherwise false
     */
    public static boolean isSupported() {
        return Utils.fileExists(FILE);
    }

    /**
     * Restores touchlight activation status setting from SharedPreferences and
     * stores it to the related kernel sysfs parameter
     * @param context The context to read the SharedPreferences from
     */
    public static void restore(Context context) {
        if (isSupported()) {
            SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);
            Utils.writeValue(FILE, sharedPrefs.getBoolean(DeviceSettings.KEY_TOUCHLIGHT_STATUS, true) ? "1" : "0");
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Utils.writeValue(FILE, ((Boolean)newValue) ? "1" : "0");
        return true;
    }

}
