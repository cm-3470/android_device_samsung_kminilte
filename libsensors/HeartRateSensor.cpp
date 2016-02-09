/*
 * Copyright (C) 2011 Samsung
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
#include <pthread.h>

#include "HeartRateSensor.h"

static void *lib_hrEol;
static void (*start_lib_ready)();
static void (*stop_lib_ready)();

static void LoadLibrary() {
    lib_hrEol = dlopen("libHrmEol.so", RTLD_NOW);
    if (!lib_hrEol) {
        LOGE("Failed to open libHrmEol.so: %s", dlerror());
    } else {
        start_lib_ready = dlsym(lib_hrEol, "start_lib_ready");
        stop_lib_ready = dlsym(lib_hrEol, "stop_lib_ready");
        if (!start_lib_ready || !stop_lib_ready) {
            LOGE("Failed to open libHrmEol.so: %s", dlerror());
        }
    }
}

HeartRateSensor::HeartRateSensor() : 
    SamsungSensorBase(NULL, "hrm_sensor", REL_DIAL),
    start_lib_ready_func(0),
    stop_lib_ready_func(0)
{
    LoadLibrary();    
    mPendingEvent.sensor = ID_HR;
    mPendingEvent.type = SENSOR_TYPE_HEART_RATE;
}

HeartRateSensor::~HeartRateSensor()
{
    if (mHrEolLib) {
        dlclose(mHrEolLib);
    }
}

int HeartRateSensor::enable(int32_t handle, int en)
{
    if (en) {
        if (!start_lib_ready)
            return -1;
        start_lib_ready();
    } else {
        if (stop_lib_ready)
            return -1;
        stop_lib_ready();
    }
    return SamsungSensorBase::enable(handle, en);
}

sensors_event_t* mPendingEvent

#define S_SAMP_XY_A         0
#define S_SAMP_XY_AB        1
#define S_SAMP_XY_B         2

void sync() 
{
    for (int i = 0; i < 10; ++i) {
        mPendingEvent->data[i] = 0;
    }

    if (mSubMode == S_SAMP_XY_A) {
        for (int i = 0; i < 4; ++i) {
            mPendingEvent->data[i] = (float)mRawBuffer[i];
        }
    } else if (mSubMode == S_SAMP_XY_AB) {
        mPendingEvent->data[0] = (float)mSumSlotB;
        mPendingEvent->data[1] = (float)/*?*/;
        for (int i = 2; i < 7; ++i) {
            mPendingEvent->data[i] = (float)mRawBuffer[i-2];
        }
        for (int i = 0; i < 11; ++i) {
            mPendingEvent->data[i+6] = (float)mRawBuffer[i];
        }
    } else if (mSubMode == S_SAMP_XY_B) {
        for (int i = 2; i < 6; ++i) {
                r2++;
                mPendingEvent->data[i] = (float)mRawBuffer[i-2];
        }
    }    
}

bool HeartRateSensor::handleEvent(input_event const *event) 
{
    //See kernel drivers/sensors/adpd142.c adpd142_sample_event() for more info
    
    if (event->type == EV_REL) {
        if (event->code == REL_X) {
            mSumSlotA = event->value - 1;
        } else if (event->code == REL_Y) {
            mSumSlotB = event->value - 1;
        } else if (event->code == REL_Z) {
            mSubMode = event->value - 1;
        }
    } else if (event->type == EV_MSC) {
        if (event->code == MSC_RAW) {
            mRawBuffer[mRawBufferIndex] = event->value - 0x1;
        }
    } else (event->type == EV_SYN) {
        mRawBufferIndex = 0;
        return sync();
    } 
    
    __android_log_print();
    return false;
}
