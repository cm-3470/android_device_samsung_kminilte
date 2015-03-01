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
import android.preference.PreferenceFragment;

import com.cyanogenmod.settings.device.R;

public class PerformanceFragmentActivity extends PreferenceFragment {

    private CheckBoxPreference mHighEndGfxStatus;
    private CheckBoxPreference mLowRamStatus;
    private SeekBarPreference mMaxBackgroundApps;
    private SeekBarPreference mMaxBackgroundServices;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addPreferencesFromResource(R.xml.performance_preferences);

        mHighEndGfxStatus = (CheckBoxPreference) findPreference(DeviceSettings.KEY_HIGHEND_GFX);
        mHighEndGfxStatus.setEnabled(HighEndGfx.isSupported());
        mHighEndGfxStatus.setOnPreferenceChangeListener(new HighEndGfx());

        mLowRamStatus = (CheckBoxPreference) findPreference(DeviceSettings.KEY_LOW_RAM);
        mLowRamStatus.setEnabled(LowRam.isSupported());
        mLowRamStatus.setOnPreferenceChangeListener(new LowRam());

        mMaxBackgroundApps = (SeekBarPreference) findPreference(DeviceSettings.KEY_MAX_BG_APPS);
        mMaxBackgroundApps.setEnabled(MaxBackgroundApps.isSupported());
        mMaxBackgroundApps.setOnPreferenceChangeListener(new MaxBackgroundApps());

        mMaxBackgroundServices = (SeekBarPreference) findPreference(DeviceSettings.KEY_MAX_BG_SERVICES);
        mMaxBackgroundServices.setEnabled(MaxBackgroundServices.isSupported());
        mMaxBackgroundServices.setOnPreferenceChangeListener(new MaxBackgroundServices());
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mHighEndGfxStatus.setChecked(HighEndGfx.getPropertyValue());
        mLowRamStatus.setChecked(LowRam.getPropertyValue());
        mMaxBackgroundApps.setValue(MaxBackgroundApps.getPropertyValue());
        mMaxBackgroundServices.setValue(MaxBackgroundServices.getPropertyValue());
    }

    public static void restore(Context context) {
        MaxBackgroundApps.restore(context);
        MaxBackgroundServices.restore(context);
    }
}
