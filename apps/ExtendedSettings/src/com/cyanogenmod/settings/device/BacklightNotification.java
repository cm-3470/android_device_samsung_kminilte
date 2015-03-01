/*
 * Copyright (C) 2014 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.cyanogenmod.settings.device;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceManager;

/**
 * Class for BLN settings
 */
public class BacklightNotification implements OnPreferenceChangeListener {

    // File paths for BLN settings
    private static final String BLN_ENABLED_FILE = "/sys/class/misc/backlightnotification/enabled";
    private static final String BLINK_ENABLED_FILE = "/sys/class/misc/backlightnotification/in_kernel_blink";
    private static final String BLINK_INTERVAL_ON_FILE = "/sys/class/misc/backlightnotification/blink_interval_on";
    private static final String BLINK_INTERVAL_OFF_FILE = "/sys/class/misc/backlightnotification/blink_interval_off";
    private static final String BLINK_TIMEOUT_FILE = "/sys/class/misc/backlightnotification/blink_maxtime";
    private static final String STATIC_ENABLED_FILE = "/sys/class/misc/backlightnotification/in_kernel_static";
    private static final String STATIC_TIMEOUT_FILE = "/sys/class/misc/backlightnotification/static_maxtime";
    private static final String STATUS_AFTER_BLINKING_FILE = "/sys/class/misc/backlightnotification/status_after_blinking";

    // Enum for BLN notification type
    private enum BlnType {
        NONE,
        BLINK,
        STATIC,
        BLINK_AND_STATIC
    }

    // Constants for toggable settings
    private static final int DISABLED = 0;
    private static final int ENABLED = 1;

    // Constants for default values
    private static final int DEFAULT_BLINK_TIMEOUT = 60;
    private static final int DEFAULT_BLINK_INTERVAL = 500;
    private static final int DEFAULT_STATIC_TIMEOUT = 60;

    /**
     * Checks if the feature is supported
     * @return True if the feature is supported, otherwise false
     */
    public static boolean isSupported() {
        return Utils.fileExists(BLN_ENABLED_FILE);
    }

    /**
     * Checks if notification with blinking lights is supported
     * @return True if the feature is supported, otherwise false
     */
    public static boolean isBlinkLightsSupported() {
        return Utils.fileExists(BLINK_ENABLED_FILE);
    }

    /**
     * Checks if notification with static lights is supported
     * @return True if the feature is supported, otherwise false
     */
    public static boolean isStaticLightsSupported() {
        return Utils.fileExists(STATIC_ENABLED_FILE);
    }

    /**
     * Checks if notification with blinking lights is enabled
     * @return True if the feature is enabled, otherwise false
     */
    public static boolean isBlinkLightsEnabled() {
        return isBlinkLightsSupported() &&
               Integer.parseInt(Utils.readValue(BLINK_ENABLED_FILE)) == ENABLED;
    }

    /**
     * Checks if notification with blinking lights is enabled
     * @return True if the feature is enabled, otherwise false
     */
    public static boolean isStaticLightsEnabled() {
        return (isStaticLightsSupported() &&
                Integer.parseInt(Utils.readValue(STATIC_ENABLED_FILE)) == ENABLED) ||
               (Utils.fileExists(STATUS_AFTER_BLINKING_FILE) &&
                Integer.parseInt(Utils.readValue(STATUS_AFTER_BLINKING_FILE)) == ENABLED);
    }

    /**
     * Restores BLN settings from SharedPreferences and store them to the related kernel sysfs parameters
     * @param context The context to read the SharedPreferences from
     */
    public static void restore(Context context) {
        // If BLN is supported
        if (isSupported()) {
            // Get the shared preferences
            SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);

            // Restore BLN notification type settings only if not disabled, to avoid the
            // overwriting of other app settings
            BlnType blnType = BlnType.NONE;
            try {
                blnType = BlnType.valueOf(sharedPrefs.getString(DeviceSettings.KEY_BLN_TYPE, BlnType.NONE.name()));
            } catch(Exception e) {}
            if (!blnType.equals(BlnType.NONE))
                storeBlnType(blnType);

            // Restore static timeout, blinking timeout and interval
            if (Utils.fileExists(BLINK_TIMEOUT_FILE))
                Utils.writeValue(BLINK_TIMEOUT_FILE, sharedPrefs.getString(DeviceSettings.KEY_BLN_BLINK_TIMEOUT, Integer.toString(DEFAULT_BLINK_TIMEOUT)));
            if (Utils.fileExists(BLINK_INTERVAL_ON_FILE))
                Utils.writeValue(BLINK_INTERVAL_ON_FILE, sharedPrefs.getString(DeviceSettings.KEY_BLN_BLINK_INTERVAL_ON, Integer.toString(DEFAULT_BLINK_INTERVAL)));
            if (Utils.fileExists(BLINK_INTERVAL_OFF_FILE))
                Utils.writeValue(BLINK_INTERVAL_OFF_FILE, sharedPrefs.getString(DeviceSettings.KEY_BLN_BLINK_INTERVAL_OFF, Integer.toString(DEFAULT_BLINK_INTERVAL)));
            if (Utils.fileExists(STATIC_TIMEOUT_FILE))
                Utils.writeValue(STATIC_TIMEOUT_FILE, sharedPrefs.getString(DeviceSettings.KEY_BLN_STATIC_TIMEOUT, Integer.toString(DEFAULT_STATIC_TIMEOUT)));
        }
    }

    /**
     * Stores the BLN settings related to feature, blinking and static activations
     * according to the notfication type requested
     * @param blnType BLN notification type
     */
    private static void storeBlnType(BlnType blnType) {
        int values[] = new int[4];

        switch (blnType) {
            case NONE:
                // BLN disabled
                values[0] = DISABLED;
                values[1] = DISABLED;
                values[2] = DISABLED;
                values[3] = DISABLED;
                break;
            case BLINK:
                // Blinking lights notifications
                values[0] = ENABLED;
                values[1] = ENABLED;
                values[2] = DISABLED;
                values[3] = DISABLED;
                break;
            case STATIC:
                // Static lights notifications
                values[0] = ENABLED;
                values[1] = DISABLED;
                values[2] = ENABLED;
                values[3] = DISABLED;
                break;
            case BLINK_AND_STATIC:
                // Blinking and then static lights notifications
                values[0] = ENABLED;
                values[1] = ENABLED;
                values[2] = DISABLED;
                values[3] = ENABLED;
                break;
            default:
                // Exit and don't store anything
                return;
        }

        // Store the settings
        Utils.writeValue(BLN_ENABLED_FILE, Integer.toString(values[0]));
        if (Utils.fileExists(BLINK_ENABLED_FILE))
            Utils.writeValue(BLINK_ENABLED_FILE, Integer.toString(values[1]));
        if (Utils.fileExists(STATIC_ENABLED_FILE))
            Utils.writeValue(STATIC_ENABLED_FILE, Integer.toString(values[2]));
        if (Utils.fileExists(STATUS_AFTER_BLINKING_FILE))
            Utils.writeValue(STATUS_AFTER_BLINKING_FILE, Integer.toString(values[3]));
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object value) {
        String key = preference.getKey();
        if (key.equals(DeviceSettings.KEY_BLN_TYPE))
            storeBlnType(BlnType.valueOf((String)value));
        else if (key.equals(DeviceSettings.KEY_BLN_BLINK_TIMEOUT))
            Utils.writeValue(BLINK_TIMEOUT_FILE, (String)value);
        else if (key.equals(DeviceSettings.KEY_BLN_BLINK_INTERVAL_ON))
            Utils.writeValue(BLINK_INTERVAL_ON_FILE, (String)value);
        else if (key.equals(DeviceSettings.KEY_BLN_BLINK_INTERVAL_OFF))
            Utils.writeValue(BLINK_INTERVAL_OFF_FILE, (String)value);
        else if (key.equals(DeviceSettings.KEY_BLN_STATIC_TIMEOUT))
            Utils.writeValue(STATIC_TIMEOUT_FILE, (String)value);
        return true;
    }
}
