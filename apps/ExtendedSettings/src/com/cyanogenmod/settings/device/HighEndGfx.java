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
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;

import com.cyanogenmod.settings.device.R;

public class HighEndGfx implements OnPreferenceChangeListener {

    // Keys of used system properties
    private static final String HIGHEND_GFX_PROP_KEY = "persist.sys.force_highendgfx";

    // HighEnd Gfx property is supported starting from Android 4.4 (KitKat)
    private static final float MIN_VERSION_SUPPORTED = 4.4f;

    /**
     * Checks if the feature is supported
     * @return True if the feature is supported, otherwise false
     */
    public static boolean isSupported() {
        return BuildProp.isSupportedSystem(MIN_VERSION_SUPPORTED);
    }

    /**
     * Gets the current value of HighEndGfx property inside system properties
     * @return The HighEndGfx value currently set
     */
    public static boolean getPropertyValue() {
        return SystemProperties.getBoolean(HIGHEND_GFX_PROP_KEY, false);
    }

    /**
     * Stores the value of HighEndGfx property inside system properties
     * @param value The value to store
     * @return True if the store succeeded, otherwise false
     */
    private static boolean storePropertyValue(Boolean value) {
        return BuildProp.set(HIGHEND_GFX_PROP_KEY, value);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object value) {
        // Get the preference context
        final Context context = preference.getContext();

        // Store the new value, if it fails abort Preference update
        if (storePropertyValue((Boolean)value)) {
            Utils.showRebootDialog(context, R.string.highend_gfx_title);
            return true;
        } else {
            Utils.showErrorDialog(context, R.string.highend_gfx_title, R.string.highend_gfx_error);
            return false;
        }
    }

}
