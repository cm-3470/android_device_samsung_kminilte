/*
 * Copyright (C) 2013 The CyanogenMod Project
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
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;

import com.cyanogenmod.settings.device.R;

public class TouchKeyFragmentActivity extends PreferenceFragment {

    private ListPreference mBacklightTimeout;
    private CheckBoxPreference mTouchLightStatus;
    private Preference mPkfManager;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addPreferencesFromResource(R.xml.touchkey_preferences);

        Context mContext = getActivity();

        mBacklightTimeout = (ListPreference) findPreference(DeviceSettings.KEY_BACKLIGHT_TIMEOUT);
        mBacklightTimeout.setEnabled(TouchKeyBacklightTimeout.isSupported());
        mBacklightTimeout.setOnPreferenceChangeListener(new TouchKeyBacklightTimeout());

        mTouchLightStatus = (CheckBoxPreference) findPreference(DeviceSettings.KEY_TOUCHLIGHT_STATUS);
        mTouchLightStatus.setEnabled(TouchLightStatus.isSupported());
        mTouchLightStatus.setOnPreferenceChangeListener(new TouchLightStatus());

        mPkfManager = (Preference) findPreference(DeviceSettings.KEY_PKFMANAGER);
        mPkfManager.setOnPreferenceClickListener(new PkfManager(mContext));
    }

    public static void restore(Context context) {
        TouchKeyBacklightTimeout.restore(context);
        TouchLightStatus.restore(context);
    }
}
