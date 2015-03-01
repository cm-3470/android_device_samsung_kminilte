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
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.PreferenceFragment;

import com.cyanogenmod.settings.device.R;

/**
 * Class for the activity fragment of BLN settings
 */
public class BacklightNotificationFragmentActivity
    extends PreferenceFragment
    implements OnSharedPreferenceChangeListener {

    private ListPreference _blnType;          // Preference for BLN notification type
    private ListPreference _blinkTimeout;     // Preference for blinking lights timeout
    private ListPreference _blinkIntervalOn;  // Preference for blink ON interval
    private ListPreference _blinkIntervalOff; // Preference for blink OFF interval
    private ListPreference _staticTimeout;    // Preference for static lights timeout

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Create the preferences according to xml resource
        addPreferencesFromResource(R.xml.bln_preferences);

        // Set change listeners
        _blnType = (ListPreference) findPreference(DeviceSettings.KEY_BLN_TYPE);
        _blnType.setOnPreferenceChangeListener(new BacklightNotification());
        _blinkTimeout = (ListPreference) findPreference(DeviceSettings.KEY_BLN_BLINK_TIMEOUT);
        _blinkTimeout.setOnPreferenceChangeListener(new BacklightNotification());
        _blinkIntervalOn = (ListPreference) findPreference(DeviceSettings.KEY_BLN_BLINK_INTERVAL_ON);
        _blinkIntervalOn.setOnPreferenceChangeListener(new BacklightNotification());
        _blinkIntervalOff = (ListPreference) findPreference(DeviceSettings.KEY_BLN_BLINK_INTERVAL_OFF);
        _blinkIntervalOff.setOnPreferenceChangeListener(new BacklightNotification());
        _staticTimeout = (ListPreference) findPreference(DeviceSettings.KEY_BLN_STATIC_TIMEOUT);
        _staticTimeout.setOnPreferenceChangeListener(new BacklightNotification());
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        // Be sure that the saved settings are restored
        BacklightNotification.restore(this.getActivity().getApplicationContext());

        // Set visibility of the available preferences
        setPreferenceVisibility();
    }

    /**
     * Sets the visibility of the available preferences
     * @param blnType BLN notification type
     */
    private void setPreferenceVisibility() {
        _blnType.setEnabled(BacklightNotification.isSupported());
        _blinkTimeout.setEnabled(BacklightNotification.isBlinkLightsEnabled());
        _blinkIntervalOn.setEnabled(BacklightNotification.isBlinkLightsEnabled());
        _blinkIntervalOff.setEnabled(BacklightNotification.isBlinkLightsEnabled());
        _staticTimeout.setEnabled(BacklightNotification.isStaticLightsEnabled());
    }

    /**
     * Restores BLN settings from SharedPreferences and store them to the related kernel sysfs parameters
     * @param context The context to read the SharedPreferences from
     */
    public static void restore(Context context) {
        BacklightNotification.restore(context);
    }

    @Override
    public void onResume() {
        super.onResume();
        getPreferenceManager().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
    }

    @Override
    public void onPause() {
        getPreferenceManager().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);
        super.onPause();
    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        // If the BLN notification type has been changed,
        // update the visibility of the preferences
        if (key.equals(DeviceSettings.KEY_BLN_TYPE)) {
            setPreferenceVisibility();
        }
    }
}
