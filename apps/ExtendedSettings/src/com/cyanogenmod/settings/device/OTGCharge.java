package com.cyanogenmod.settings.device;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceManager;

public class OTGCharge implements OnPreferenceChangeListener {

    private static final String FILE = "/sys/class/sec/switch/otg_cable_type";
    private static final String DEFAULT_OTG_POWER_MODE = "BATTERY";

    public OTGCharge() {
    }

    public static boolean isSupported() {
        return Utils.fileExists(FILE);
    }

    /**
     * Restore OTG charge setting (Write to kernel.)
     * @param context       The context to read the SharedPreferences from
     */
    public static void restore(Context context) {
        if (isSupported()) {
            SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);
            String mode = sharedPrefs.getString(DeviceSettings.KEY_OTG_POWER, DEFAULT_OTG_POWER_MODE);
            if (!mode.equals(DEFAULT_OTG_POWER_MODE))
                Utils.writeValue(FILE, mode);
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Utils.writeValue(FILE, (String)newValue);
        return true;
    }
}

