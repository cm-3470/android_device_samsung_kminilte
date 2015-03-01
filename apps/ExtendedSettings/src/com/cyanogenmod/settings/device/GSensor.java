package com.cyanogenmod.settings.device;

import android.os.Handler;
import android.os.Bundle;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.app.Activity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;
import android.util.Log;

public class GSensor extends Activity implements SensorEventListener {

    private static final String FILE = "/sys/class/input/event5/device/calibration";
    private static final String TAG = "GSensor";
    private Button calibrateButton;
    private SensorManager sm = null;

    public static boolean isSupported() {
        return Utils.fileExists(FILE);
    }

    public void startCalibration() {
        if (isSupported()) {
            handler.post(runnable);
        }
    }

    private Runnable runnable = new Runnable() {
        public void run() {
            Utils.writeValue(FILE, "1");
            runOnUiThread(new Runnable() {
                public void run() {
                    calibrateButton.setText(R.string.gsensor_button1);
                    calibrateButton.setEnabled(false);
                    showToast();
                }
            });
        }
    };

    private Handler handler = new Handler();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.gsensor);
        sm = (SensorManager) getSystemService(SENSOR_SERVICE);
        calibrateButton = (Button)findViewById(R.id.gsensor_button);
        calibrateButton.setOnClickListener(new View.OnClickListener() {
             public void onClick(View v) {
                 startCalibration();
             }
        });
   }

    @Override
    protected void onResume() {
        super.onResume();
        Sensor Accel = sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        sm.registerListener((SensorEventListener) this, Accel, SensorManager.SENSOR_DELAY_FASTEST);
        calibrateButton.setText(R.string.gsensor_button2);
        calibrateButton.setEnabled(true);
    }

    @Override
    protected void onStop() {
        sm.unregisterListener(this);
        super.onStop();
    }

    private void showToast() {
         Toast.makeText(this, getString(R.string.gsensor_end), Toast.LENGTH_SHORT).show();
    }

    public void onSensorChanged(SensorEvent event) {
        Sensor sensor = event.sensor;
        float [] values = event.values;
        Log.d(TAG,String.format("onSensorChanged: %s, values: %f %f %f",sensor.toString(),values[0],values[1],values[2]));
    }

    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        Log.d(TAG,String.format("onAccuracyChanged: %s, accuracy: %d",sensor.toString(),accuracy));
    }

}

