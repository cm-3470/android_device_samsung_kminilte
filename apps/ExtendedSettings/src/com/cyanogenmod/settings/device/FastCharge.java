package com.cyanogenmod.settings.device;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.widget.Toast;

public class FastCharge implements OnPreferenceChangeListener {

    private static final String FILE = "/sys/kernel/fast_charge/force_fast_charge";
    private Context mContext;

    public FastCharge(Context context) {
        mContext = context;
    }

    public static String getFilePath() {
        return FILE;
    }

    public static boolean isSupported() {
        return Utils.fileExists(FILE);
    }

    public static boolean isActive() {
        return isSupported() && (Utils.readValue(FILE)).equals("1");
    }

    /**
     * Restore fast charge setting from SharedPreferences. (Write to kernel.)
     * @param context       The context to read the SharedPreferences from
     */
    public static void restore(Context context) {
        if (!isSupported()) {
            return;
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Intent intent = mContext.registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
        int plugged = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1);
        boolean USBIsPlugged = (plugged == BatteryManager.BATTERY_PLUGGED_AC || plugged == BatteryManager.BATTERY_PLUGGED_USB);

        if (USBIsPlugged == true) {
            Toast.makeText(mContext, mContext.getString(R.string.force_fast_charge_usbwarning), Toast.LENGTH_SHORT).show();
            return false;
        } else {
            Utils.writeValue(FILE, ((Boolean)newValue) ? "1" : "0");
            return true;
        }
    }

}

