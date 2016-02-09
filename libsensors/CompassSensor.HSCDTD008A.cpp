/*
 * Copyright (C) 2012 The Android Open Source Project
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

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>
#include <linux/input.h>
#include <dlfcn.h>

#include "sensor_params.h"
#include "MPLSupport.h"

#include "CompassSensor.HSCDTD008A.h"

#define COMPASS_EVENT_DEBUG 0

typedef int (*Magnetic_Enable_func)(void);
typedef int (*Magnetic_Disable_func)(void);
typedef int (*Magnetic_Calibrate_func)(sensor_data_t *in, sensors_vec_t *out);
typedef int (*Magnetic_Set_Delay_func)(uint64_t delay);
typedef int (*Magnetic_Initialize_func)();

static void *lib_acdapi_clb = 0;
static Magnetic_Enable_func Magnetic_Enable = 0;
static Magnetic_Disable_func Magnetic_Disable = 0;
static Magnetic_Calibrate_func Magnetic_Calibrate = 0;
static Magnetic_Set_Delay_func Magnetic_Set_Delay = 0;
static Magnetic_Initialize_func Magnetic_Initialize = 0;

static void LoadLibrary() {
    lib_acdapi_clb = dlopen("libacdapi_clb.so", RTLD_NOW);
    if (!lib_acdapi_clb) {
        LOGE("Failed to open libacdapi_clb.so: %s", dlerror());
    } else {
        Magnetic_Enable = (Magnetic_Enable_func)dlsym(lib_acdapi_clb, "Magnetic_Enable");
        Magnetic_Disable = (Magnetic_Disable_func)dlsym(lib_acdapi_clb, "Magnetic_Disable");
        Magnetic_Calibrate = (Magnetic_Calibrate_func)dlsym(lib_acdapi_clb, "Magnetic_Calibrate");
        Magnetic_Set_Delay = (Magnetic_Set_Delay_func)dlsym(lib_acdapi_clb, "Magnetic_Set_Delay");
        Magnetic_Initialize = (Magnetic_Initialize_func)dlsym(lib_acdapi_clb, "Magnetic_Initialize");
        if (!Magnetic_Disable || !Magnetic_Enable || !Magnetic_Calibrate || 
            !Magnetic_Set_Delay || !Magnetic_Initialize) {
            LOGE("Failed to open libacdapi_clb.so: %s", dlerror());
        }
    }
}

/*****************************************************************************/

CompassSensor::CompassSensor() :
    SamsungSensorBase(NULL, "magnetic_sensor"),
    mAccuracy(0)
{
    VFUNC_LOG;
    LoadLibrary();
    Magnetic_Initialize();
    memset(&mCachedCompassData, 0, sizeof(mCachedCompassData));   
}

CompassSensor::~CompassSensor()
{
    VFUNC_LOG;
}

int CompassSensor::enable(int32_t handle, int en)
{
    LOGI_IF(COMPASS_EVENT_DEBUG, "enable compass: %ld", en);
    
    if (handle == ID_RM)
        mRaw = true;
    
    if (en) {
        if (!Magnetic_Enable)
            return -1;
        Magnetic_Enable();
    } else {
        if (!Magnetic_Disable)
            return -1;
        Magnetic_Disable();
    }
    return SamsungSensorBase::enable(handle, en);
}

/**
    @brief      This function will return the state of the sensor.
    @return     1=enabled; 0=disabled
**/
int CompassSensor::getEnable(int32_t handle)
{
    VFUNC_LOG;
    return mEnabled;
}

/**
    @brief      This function will return the current delay for this sensor.
    @return     delay in nanoseconds. 
**/
int64_t CompassSensor::getDelay(int32_t handle)
{
    VFUNC_LOG;
    return mDelay;
}

int CompassSensor::setDelay(int32_t handle, int64_t ns)
{
    // TODO: does Magnetic_Set_Delay() expect ms?
    LOGI_IF(COMPASS_EVENT_DEBUG, "Set delay: %ld", ns);    
    Magnetic_Set_Delay(ns/1000000);
    return SamsungSensorBase::setDelay(handle, ns);
}

bool CompassSensor::handleEvent(input_event const *event) 
{
    if (event->type == EV_REL) {
        if (event->code == EVENT_TYPE_REL_MAGV_X) {      
            LOGI_IF(COMPASS_EVENT_DEBUG, "EVENT_TYPE_MAGV_X\n");
            mCachedCompassData.x = event->value;
        } else if (event->code == EVENT_TYPE_REL_MAGV_Y) {
            LOGI_IF(COMPASS_EVENT_DEBUG, "EVENT_TYPE_MAGV_Y\n");
            mCachedCompassData.y = event->value;
        } else if (event->code == EVENT_TYPE_REL_MAGV_Z) {
            LOGI_IF(COMPASS_EVENT_DEBUG, "EVENT_TYPE_MAGV_Z\n");
            mCachedCompassData.z = event->value;
        }
    } else if (event->type == EV_SYN) {
        sensors_vec_t out;        
        if (Magnetic_Calibrate(&mCachedCompassData, &out)) {
            LOGI_IF(COMPASS_EVENT_DEBUG, "Magnetic_Calibrate (raw:%d): (%d/%d/%d) -> (%f/%f/%f %d)\n",
                mRaw, mCachedCompassData.x, mCachedCompassData.y, mCachedCompassData.z,
                out.x, out.y, out.z, out.status);
            
            mAccuracy = out.status;
            
            if (mRaw) {
                mPendingEvent.uncalibrated_magnetic.x_uncalib = (float)mCachedCompassData.x;
                mPendingEvent.uncalibrated_magnetic.y_uncalib = (float)mCachedCompassData.y;
                mPendingEvent.uncalibrated_magnetic.z_uncalib = (float)mCachedCompassData.z;
                mPendingEvent.uncalibrated_magnetic.x_bias = (float)mCachedCompassData.x - out.x;
                mPendingEvent.uncalibrated_magnetic.y_bias = (float)mCachedCompassData.y - out.y;
                mPendingEvent.uncalibrated_magnetic.z_bias = (float)mCachedCompassData.z - out.z;
            } else {
                mPendingEvent.magnetic.x = out.x;
                mPendingEvent.magnetic.y = out.y;
                mPendingEvent.magnetic.z = out.z;
                mPendingEvent.magnetic.status = out.status;
            }
            mHasPendingEvent = true;
        }        
        return false;
    }    
    
    // no event created
    return false;
}

/**
    @brief         Integrators need to implement this function per 3rd-party solution
    @param[out]    data      sensor data is stored in this variable. Scaled such that
                             1 uT = 2^16
    @para[in]      timestamp data's timestamp
    @return        1, if 1   sample read, 0, if not, negative if error
**/
int CompassSensor::readSample(long *data, int64_t *timestamp)
{
    VFUNC_LOG;
    sensors_event_t event;
    
    int res = readEvents(&event, 1);
    if (res > 0) {
        *timestamp = mPendingEvent.timestamp;
        for(int i=0; i<3; i++) {
            data[i] = (long)(mPendingEvent.magnetic.v[i] * 65536.0);
        }
        LOGI_IF(COMPASS_EVENT_DEBUG, "readSample: (%d/%d/%d)",
            data[0], data[1], data[2]);
        return 1;
    }
    return res;
}

/**
    @brief         Integrators need to implement this function per 3rd-party solution
    @param[out]    data      sensor data is stored in this variable. 
    @para[in]      timestamp data's timestamp
    @return        1, if 1   sample read, 0, if not, negative if error
**/
int CompassSensor::readRawSample(float *data, int64_t *timestamp)
{
    VFUNC_LOG;

    sensors_event_t event;
    
    int res = readEvents(&event, 1);
    if (res > 0) {
        *timestamp = mPendingEvent.timestamp;
        for(int i=0; i<3; i++) {
            data[i] = mPendingEvent.uncalibrated_magnetic.uncalib[i];
        }
        LOGI_IF(COMPASS_EVENT_DEBUG, "readSample (raw): (%f/%f/%f)", data[0], data[1], data[2]);
        return 1;
    }
    return res;
}

void CompassSensor::fillList(struct sensor_t *list)
{
    VFUNC_LOG;

    list->maxRange = 0.0;
    list->resolution = 0.0;
    list->power = 0.0;
    list->minDelay = 10000;
}

// specifies compass sensor's mounting matrix for MPL
void CompassSensor::getOrientationMatrix(signed char *orient)
{
    VFUNC_LOG;

    orient[0] = 0;
    orient[1] = -1;
    orient[2] = 0;
    orient[3] = 1;
    orient[4] = 0;
    orient[5] = 0;
    orient[6] = 0;
    orient[7] = 0;
    orient[8] = 1;
}

int CompassSensor::getAccuracy(void)
{
    VFUNC_LOG;
    return mAccuracy;
}
