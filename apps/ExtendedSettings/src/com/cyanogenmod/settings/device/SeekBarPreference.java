/**
 *
 * Copyright (C) 2013-2015, Cristoforo Cataldo (Christopher83)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
package com.cyanogenmod.settings.device;

import android.content.Context;
import android.content.res.Resources.NotFoundException;
import android.preference.DialogPreference;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

/**
 * Class for SeekBar preference dialog
 * @author Cristoforo Cataldo (Christopher83)
 */
public class SeekBarPreference extends DialogPreference implements SeekBar.OnSeekBarChangeListener {

    // Constants for the attribute namespaces
    private static final String ANDROID_NAMESPACE   = "http://schemas.android.com/apk/res/android";
    private static final String APP_NAMESPACE       = "http://cyanogenmod";

    // Constants for the attribute names
    private static final String ATTR_DEFAULT_VALUE  = "defaultValue";
    private static final String ATTR_MIN_VALUE      = "minValue";
    private static final String ATTR_MAX_VALUE      = "maxValue";
    private static final String ATTR_STEP_VALUE     = "stepValue";
    private static final String ATTR_POSTFIX_SYMBOL = "postfixSymbol";

    // Constants for default min, max and step values
    private static final int DEFAULT_MIN_VALUE      = 0;
    private static final int DEFAULT_MAX_VALUE      = 100;
    private static final int DEFAULT_STEP_VALUE     = 1;

    private SeekBar mSeekBar;                       // SeekBar control
    private TextView mDialogMessageView;            // TextView for dialog control
    private TextView mDialogValueView;              // TextView for current preference value
    private TextView mDialogMinValueView;           // TextView for min preference value
    private TextView mDialogMaxValueView;           // TextView for max preference value
    private int mValue = DEFAULT_MIN_VALUE;         // Current value
    private int mDefaultValue = DEFAULT_MIN_VALUE;  // Default value
    private int mMinValue = DEFAULT_MIN_VALUE;      // Min allowed value
    private int mMaxValue = DEFAULT_MAX_VALUE;      // Max allowed value
    private int mStepValue = DEFAULT_STEP_VALUE;    // Increasing/decreasing step value for SeekBar control
    private String mPostfixSymbol;                  // Postfix symbol to append after the showed current value

    /**
     * Class constructor
     * @param context The application context
     * @param attrs The initialization attributes
     */
    public SeekBarPreference(Context context, AttributeSet attrs) {
        // Invoke the base class constructor
        super(context, attrs);

        // Initialize the attributes of preference control
        initialize(context, attrs);
    }

    /**
     * Class constructor
     * @param context The application context
     * @param attrs The initialization attributes
     * @param defStyle The resource ID of the default style
     */
    public SeekBarPreference(Context context, AttributeSet attrs, int defStyle) {
        // Invoke the base class constructor
        super(context, attrs, defStyle);

        // Initialize the attributes of preference control
        initialize(context, attrs);
    }

    /**
     * Initializes the attributes of preference control
     * @param context The application context
     * @param attrs The initialization attributes
     */
    private void initialize(Context context, AttributeSet attrs) {
        // If there aren't attributes then exit
        if (attrs == null)
            return;

        // Get the attributes of the preference control set inside the resource file and store them
        mDefaultValue = getAttributeIntValue(context, attrs, ANDROID_NAMESPACE, ATTR_DEFAULT_VALUE, DEFAULT_MIN_VALUE);
        mMinValue = getAttributeIntValue(context, attrs, APP_NAMESPACE, ATTR_MIN_VALUE, DEFAULT_MIN_VALUE);
        mMaxValue = getAttributeIntValue(context, attrs, APP_NAMESPACE, ATTR_MAX_VALUE, DEFAULT_MAX_VALUE);
        mStepValue = getAttributeIntValue(context, attrs, APP_NAMESPACE, ATTR_STEP_VALUE, DEFAULT_STEP_VALUE);
        mPostfixSymbol = getAttributeValue(context, attrs, APP_NAMESPACE, ATTR_POSTFIX_SYMBOL, -1);
    }

    /**
     * Gets the string value of a preference attribute
     * @param context The application context
     * @param attrs The initialization attributes
     * @param namespace The attribute namespace
     * @param attribute The attribute name
     * @param defaultValue The attribute default value
     * @return The preference attribute value as string
     */
    private String getAttributeValue(Context context, AttributeSet attrs, String namespace, String attribute, int defaultValue) {
        try {
            // Get and return the string value of the preference attribute from resource file
            return context.getString(attrs.getAttributeResourceValue(namespace, attribute, defaultValue));
        } catch (NotFoundException e) {
            // Get and return the string value of the preference attribute from the specified attributes
            return attrs.getAttributeValue(namespace, attribute);
        }
    }

    /**
     * Gets the integer value of a preference attribute
     * @param context The application context
     * @param attrs The initialization attributes
     * @param namespace The attribute namespace
     * @param attribute The attribute name
     * @param defaultValue The attribute default value
     * @return The preference attribute value as string
     */
    private int getAttributeIntValue(Context context, AttributeSet attrs, String namespace, String attribute, int defaultValue) {
        // Get the string value of the preference attribute from the specified attributes
        String value = attrs.getAttributeValue(namespace, attribute);

        // If the value is a resource reference
        if (value != null && value.startsWith("@")) {
            // Get and return the integer value from the referenced resource attribute
            int resourceID = attrs.getAttributeResourceValue(namespace, attribute, defaultValue);
            return (resourceID != 0) ? context.getResources().getInteger(resourceID) : defaultValue;
        } else {
            // Get and return the integer value from the specified attributes
            return attrs.getAttributeIntValue(namespace, attribute, defaultValue);
        }
    }

    /**
     * Manages the dialog view layout creation
     */
    @Override
    protected View onCreateDialogView() {
        // Get the current application context
        Context context = this.getContext();

        // Get the current preference value
        mValue = getValue();

        // Create and set the SeekBar control
        mSeekBar = new SeekBar(context);
        mSeekBar.setMax(mMaxValue - mMinValue);
        mSeekBar.setProgress(mValue - mMinValue);
        mSeekBar.setOnSeekBarChangeListener(this);

        // Create and set the TextView for the dialog title
        mDialogMessageView = new TextView(context);
        mDialogMessageView.setVisibility(View.GONE);
        mDialogMessageView.setTextSize(20);

        // Create and set the TextView for the current value
        mDialogValueView = new TextView(context);
        mDialogValueView.setTextSize(20);
        mDialogValueView.setText(getTextFromProgress(mSeekBar.getProgress()));

        // Create and set the TextView for the min allowed value
        mDialogMinValueView = new TextView(context);
        mDialogMinValueView.setTextSize(14);
        mDialogMinValueView.setText(Integer.toString(mMinValue));

        // Create and set the TextView for the max allowed value
        mDialogMaxValueView = new TextView(context);
        mDialogMaxValueView.setTextSize(14);
        mDialogMaxValueView.setText(Integer.toString(mMaxValue));

        // Padding for outer contents and inner contents
        int paddingOuter = Math.round(context.getResources().getDisplayMetrics().density * 16);
        int paddingInner = Math.round(context.getResources().getDisplayMetrics().density * 8);

        // Create the TextViews container and set the padding for inner content
        RelativeLayout valueLayout = new RelativeLayout(context);
        valueLayout.setPadding(paddingInner, paddingInner, paddingInner, paddingInner);

        // Create and set the style and content of the main container of the preference dialog
        LinearLayout layout = new LinearLayout(context);
        layout.setPadding(paddingOuter, paddingOuter, paddingOuter, paddingOuter);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.addView(mDialogMessageView, LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        layout.addView(valueLayout, LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        layout.addView(mSeekBar, LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);

        // Add and position the TextViews for the min, current, max values to the related inner container
        RelativeLayout.LayoutParams params = null;
        params = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.CENTER_VERTICAL);
        params.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
        valueLayout.addView(mDialogMinValueView, params);
        params = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        params.addRule(RelativeLayout.CENTER_HORIZONTAL);
        valueLayout.addView(mDialogValueView, params);
        params = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.CENTER_VERTICAL);
        params.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
        valueLayout.addView(mDialogMaxValueView, params);

        // Return the created layout of the dialog view
        return layout;
    }

    /**
     * Manages the dialog view binding
     * @param view The dialog view
     */
    @Override
    protected void onBindDialogView(View view) {
        // Invoke the base class method
        super.onBindDialogView(view);

        // Set the max and the current values of the SeekBar control considering the provided min value
        // Note: the SeekBar doesn't support a min value different than 0, so it will be aritmetically calculated
        mSeekBar.setMax(mMaxValue - mMinValue);
        mSeekBar.setProgress(mValue - mMinValue);
    }

    /**
     * Manages the dialog closing
     * @param The flag indicating if the confirm button has been pressed
     */
    @Override
    protected void onDialogClosed(boolean positiveResult) {
        // Invoke the base class method
        super.onDialogClosed(positiveResult);

        // If the confirm button has been pressed
        if (positiveResult) {
            // Get the current chosen value
            int value = getValueFromProgress(mSeekBar.getProgress());

            // Call the change listener, if the update is confirmed
            if (callChangeListener(Integer.valueOf(value))) {
                // Update local value
                mValue = value;

                // If the value must be persisted, then it will be stored
                if (isPersistent())
                    persistInt(value);

                // Notify that the value has been changed
                notifyChanged();
            }
        }
    }

    /**
     * Manages the initial value setting
     * @param restoreValue The flag indicating if restore the value or not
     * @param defaultValue The default value
     */
    @Override
    protected void onSetInitialValue(boolean restoreValue, Object defaultValue) {
        // Invoke the base class method
        super.onSetInitialValue(restoreValue, defaultValue);

        // If the value must be persisted, then it will be stored
        if (isPersistent())
            persistInt(restoreValue ? getValue() : (Integer)defaultValue);
    }

    /**
     * Manages the SeekBar progress changing
     * @param seekBar The SeekBar whose progress has changed
     * @param progress The current progress level
     * @param fromUser The flag indicating if the progress change was initiated by the user
     */
    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        // Set the text of the TextView for the current value
        mDialogValueView.setText(getTextFromProgress(progress));
    }

    /**
     * Manages the SeekBar tracking start
     */
    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    /**
     * Manages the SeekBar tracking stop
     */
    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
    }

    /**
     * Gets the calculated current value inside the provided min..max range and also
     * considering the provided step value from the current SeekBar progress level
     * @param progress The current progress level
     * @return The calculated current value
     */
    private int getValueFromProgress(int progress) {
        // Since the SeekBar control supports only a min value equals to 0,
        // the real current value must be aritmetically calculated inside
        // the provided min..max range, also considering the provided step value
        return ((int)Math.round(progress / mStepValue)) * mStepValue + mMinValue;
    }

    /**
     * Gets the string representation of the calculated current value
     * @param progress The current progress level
     * @return The string representation of the calculated current value
     * @see SeekBarPreference#getValueFromProgress
     */
    private String getTextFromProgress(int progress) {
        // Get the calculated value and convert it to string
        String text = String.valueOf(getValueFromProgress(progress));

        // If a postfix symbol has been provided, then append to the string
        if (mPostfixSymbol != null)
            text = text.concat(mPostfixSymbol);

        // Return the string value
        return text;
    }

    /**
     * Gets the current preference value
     * @return The current preference value
     */
    public int getValue() {
        try {
            // Return the persisted value (if it must be persisted), otherwise return the current local value
            return isPersistent() ? getPersistedInt(mDefaultValue) : mValue;
        } catch (ClassCastException e) {
            // Return the default value
            return mDefaultValue;
        }
    }

    /**
     * Sets the current preference value
     * @return The current preference value
     */
    public void setValue(int value) {
        // Update the local value
        mValue = value;

        // If the value must be persisted, then it will be stored
        if (isPersistent())
            persistInt(value);
    }
}
