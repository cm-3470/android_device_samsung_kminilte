package com.cyanogenmod.settings.device;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceManager;

public class TouchKeyBacklightTimeout implements OnPreferenceChangeListener {

    private static final String FILE = "/sys/class/misc/notification/bl_timeout";
    private static final int DEFAULT_BACKLIGHT_TIMEOUT = 1600;

    /**
     * Checks if the feature is supported
     * @return True if the feature is supported, otherwise false
     */
    public static boolean isSupported() {
        return Utils.fileExists(FILE);
    }

    /**
     * Restores backlight timeout setting from SharedPreferences and
     * stores it to the related kernel sysfs parameter
     * @param context The context to read the SharedPreferences from
     */
    public static void restore(Context context) {
        if (isSupported()) {
            SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);
            Utils.writeValue(FILE, sharedPrefs.getString(DeviceSettings.KEY_BACKLIGHT_TIMEOUT, Integer.toString(DEFAULT_BACKLIGHT_TIMEOUT)));
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Utils.writeValue(FILE, (String)newValue);
        return true;
    }
}
