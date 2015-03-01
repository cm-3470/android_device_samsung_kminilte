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

import android.os.SystemProperties;
import android.util.Log;

import com.cyanogenmod.settings.device.CommandProcessor.CommandResult;

/**
 * Build Properties Utility class
 */
public class BuildProp {

    // Logging tag
    private static final String TAG = "DeviceSettings_BuildPropUtils";

    // Build Properties file
    private static final String BUILD_PROP_FILE = "/system/build.prop";

    // Persisted properties path
    private static final String PERSISTED_PROPS_PATH = "/data/property";

    // Commands to get and store a property value
    private static final String GET_COMMAND =
            "grep '^%1$s' %2$s | sed 's|^.*=||'";
    private static final String STORE_COMMAND =
            "if grep '^%1$s' %3$s; then" +
            "    sed -i 's|^%1$s=.*|%1$s=%2$s|' %3$s; " +
            "else" +
            "    echo '%1$s=%2$s' >> %3$s; " +
            "fi; " +
            "if [[ %1$s == persist.* ]]; then" +
            "    echo -n '%2$s' > %4$s/%1$s; " +
            "fi";

    // Key for system version property
    private static final String SYSTEM_VERSION_PROP_KEY = "ro.build.version.release";

    /**
     * Gets the current value of a property stored inside system
     * properties file (/system/build.prop)
     * @param key The key of the property to get
     * @return The value currently set
     */
    public static String getFromFile(String key) {
        String value = "";

        try {
            // Prepare the command to get the current value
            String getCommand = String.format(GET_COMMAND, key, BUILD_PROP_FILE);

            CommandProcessor cmd = new CommandProcessor();

            // Try to run the command without root privileges
            CommandResult result = cmd.sh.runWaitFor(getCommand);

            // If the command failed, try to run it with root privileges
            if (!result.success()) {
                Log.i(TAG, String.format("Failed to run get command without root privileges: %s", getCommand));

                result = cmd.su.runWaitFor(getCommand);
                if (result.success()) {
                    value = result.stdout;
                }
            } else {
                value = result.stdout;
            }
        } catch(Exception exc) {
            // Log a detailed error in case of exception
            Log.e(TAG, String.format("Failed to get '%s' property, an exception occured.", key), exc);
        }

        // Return the value currently set
        return value;
    }

    /**
     * Gets the current value of a property
     * @param key The key of the property to get
     * @return The value currently set
     */
    public static String get(String key) {
        // If the property is readonly
        if (key.startsWith("ro.")) {
            // Get the value from system properties file
            String value = getFromFile(key);

            // If the property is not set inside the file, try to get it from system
            if (value == null || value.isEmpty())
                value = SystemProperties.get(key);

            return value;
        // If the property is not readonly, directly get the value from system
        } else {
            return SystemProperties.get(key);
        }
    }

    /**
     * Gets the current value of a boolean property
     * @param key The key of the property to get
     * @return The value currently set
     */
    public static boolean getBoolean(String key) {
        return Boolean.parseBoolean(get(key));
    }

    /**
     * Gets the current value of an integer property
     * @param key The key of the property to get
     * @return The value currently set
     */
    public static int getInt(String key) {
        return Integer.parseInt(get(key));
    }

    /**
     * Gets the current value of a long property
     * @param key The key of the property to get
     * @return The value currently set
     */
    public static long getLong(String key) {
        return Long.parseLong(get(key));
    }

    /**
     * Gets the current value of a float property
     * @param key The key of the property to get
     * @return The value currently set
     */
    public static float getFloat(String key) {
        return Float.parseFloat(get(key));
    }

    /**
     * Gets the current value of a double property
     * @param key The key of the property to get
     * @return The value currently set
     */
    public static double getDouble(String key) {
        return Double.parseDouble(get(key));
    }

    /**
     * Stores the value of a property
     * @param key The key of the property
     * @param value The value to store
     * @return True if the store succeeded, otherwise false
     */
    public static boolean set(String key, Object value) {
        boolean updated = false;

        try {
            // Store the value inside system properties
            SystemProperties.set(key, String.valueOf(value));

            // Mount the system partition writable
            if (CommandProcessor.getMount("rw")) {
                // Store the value inside system properties file
                CommandProcessor cmd = new CommandProcessor();
                CommandResult result = cmd.su.runWaitFor(
                    String.format(STORE_COMMAND, key, String.valueOf(value), BUILD_PROP_FILE, PERSISTED_PROPS_PATH)
                );

                // Save the exit status of store command
                updated = result.success();
            }

            // Log an error if the store failed
            if (!updated)
                Log.e(TAG, String.format("Failed to store '%s' property, check root permissions.", key));
        } catch(Exception exc) {
            // Log a detailed error in case of exception
            Log.e(TAG, String.format("Failed to get '%s' property, an exception occured.", key), exc);
        } finally {
            // Mount the system partition readonly
            CommandProcessor.getMount("ro");
        }

        // Return the update result
        return updated;
    }

    /**
     * Checks if the system is supported
     * @param minSystemVersion The minimum supported system
     * @return True if the feature is supported, otherwise false
     */
    public static boolean isSupportedSystem(float minSystemVersion) {
        // Get the system version
        String versionString = SystemProperties.get(SYSTEM_VERSION_PROP_KEY);

        // Return true if the system is equal or above the version passed as argument
        if (versionString != null && !versionString.isEmpty()) {
            int index = versionString.indexOf(".");
            float version = Float.parseFloat((index > 0 && versionString.length() >= index + 2) ?
                    versionString.substring(0, index + 2) : versionString);
            return version >= minSystemVersion;
        } else {
            return false;
        }
    }
}
