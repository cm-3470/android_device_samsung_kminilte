/*
 * Copyright (C) 2015 The CyanogenMod Project
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

import com.cyanogenmod.settings.device.R;

public class MaxBackgroundServices implements OnPreferenceChangeListener {

    // Keys of used system properties
    private static final String MAX_BG_SERVICES_PROP_KEY = "ro.config.max_starting_bg";

    // Max background services property is supported starting from Android 4.4 (KitKat)
    private static final float MIN_VERSION_SUPPORTED = 4.4f;

    // Default value
    private static final int DEFAULT_MAX_BG_SERVICES = 8;

    /**
     * Checks if the feature is supported
     * @return True if the feature is supported, otherwise false
     */
    public static boolean isSupported() {
        return BuildProp.isSupportedSystem(MIN_VERSION_SUPPORTED);
    }

    /**
     * Gets the current value of max background services property inside system properties
     * @return The max background services value currently set
     */
    public static int getPropertyValue() {
        try {
            return BuildProp.getInt(MAX_BG_SERVICES_PROP_KEY);
        } catch (Exception e) {
            return DEFAULT_MAX_BG_SERVICES;
        }
    }

    /**
     * Stores the value of max background services property inside system properties
     * @param value The value to store
     * @return True if the store succeeded, otherwise false
     */
    private static boolean storePropertyValue(Integer value) {
        return BuildProp.set(MAX_BG_SERVICES_PROP_KEY, value);
    }

    /**
     * Restores the value of max background services from shared preferences
     * @param context The context to read the SharedPreferences from
     */
    public static void restore(Context context) {
        if (isSupported()) {
            SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);
            int defaultValue = getPropertyValue();
            int value = sharedPrefs.getInt(DeviceSettings.KEY_MAX_BG_SERVICES, defaultValue);
            if (value != defaultValue)
                storePropertyValue(Integer.valueOf(value));
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object value) {
        // Get the preference context
        final Context context = preference.getContext();

        // Store the new value, if it fails abort Preference update
        if (storePropertyValue((Integer)value)) {
            Utils.showRebootDialog(context, R.string.max_bg_services_title);
            return true;
        } else {
            Utils.showErrorDialog(context, R.string.max_bg_services_title, R.string.max_bg_services_error);
            return false;
        }
    }

}
