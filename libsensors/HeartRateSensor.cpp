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
#include <dlfcn.h>
#include <algorithm>

#include "HeartRateSensor.h"

#define HR_EVENT_DEBUG 1

typedef void (*start_lib_ready_func)(void);
typedef void (*stop_lib_ready_func)(void);

static void *lib_hrEol = 0;
static start_lib_ready_func start_lib_ready = 0;
static stop_lib_ready_func stop_lib_ready = 0;

static void LoadLibrary() {
    lib_hrEol = dlopen("libHrmEol.so", RTLD_NOW);
    if (!lib_hrEol) {
        LOGE("Failed to open libHrmEol.so: %s", dlerror());
    } else {
        start_lib_ready = (start_lib_ready_func)dlsym(lib_hrEol, "start_lib_ready");
        stop_lib_ready = (stop_lib_ready_func)dlsym(lib_hrEol, "stop_lib_ready");
        if (!start_lib_ready || !stop_lib_ready) {
            LOGE("Failed to open libHrmEol.so: %s", dlerror());
        }
    }
}

HeartRateSensor::HeartRateSensor() : 
    SamsungSensorBase(NULL, "hrm_sensor"),
    mRawBufferIndex(0),
    mSumSlotA(0),
    mSumSlotB(0),
    mSubMode(0),
    mLastDetect(false),
    mBpm(0),
    mLastPulseTime(0)
{
    LoadLibrary();    
    mPendingEvent.sensor = ID_HR;
    mPendingEvent.type = SENSOR_TYPE_HEART_RATE;
    pulseDetectReset();
}

HeartRateSensor::~HeartRateSensor()
{
    if (lib_hrEol) {
        dlclose(lib_hrEol);
    }
}

int HeartRateSensor::enable(int32_t handle, int en)
{
    if (en) {
        if (!start_lib_ready)
            return -1;
        start_lib_ready();
    } else {
        if (!stop_lib_ready)
            return -1;
        stop_lib_ready();
    }
    pulseDetectReset();
    return SamsungSensorBase::enable(handle, en);
}

void HeartRateSensor::pulseDetectReset()
{
    for (int i = 0; i < 8; ++i) {
        mAvg[i] = 0;
        mMinVal[i] = 0;
        mMaxVal[i] = 0;
    }
    mLastDetect = false;
    mBpm = 0;
    mLastPulseTime = 0;
}

bool HeartRateSensor::pulseDetect(int *chanSums, int *chanData) 
{
    /*
     * The ADPD142 drives two LEDs (A and B).
     * The light spectrum of the light reflected by the finger 
     * is separated into 4 bands (channels).
     * 
     * The hrEol lib setups the kernel to always use submode S_SAMP_XY_AB.
     */
    
    bool hasEvent = false;
    
    int sumChansDetect = 0;
    for (int i = 0; i < 8; ++i) {
        int val = chanData[i];
        
        // calculate floating average, floating min and max
        mAvg[i] = (mAvg[i]*10 + val) / 11;
        mMinVal[i] = ( val <= mMinVal[i] ? val : (mMinVal[i]*10 + val) / 11 );
        mMaxVal[i] = ( val >= mMaxVal[i] ? val : (mMaxVal[i]*10 + val) / 11 );
        
        // get range between min and max
        int range = std::max(mMaxVal[i] - mMinVal[i], 0);
        int unbiasedVal = mAvg[i] - mMinVal[i];

        // scale value to range 0..10
        int scale = (range > 0 ? range : 10000000);
        int scaledVal = 10 * unbiasedVal / scale;
        
        // detect peaks (> 30%)
        if (scaledVal > 3)
            sumChansDetect++;
    }
        
    if (sumChansDetect > 4 && !mLastDetect) {
        mLastDetect = true;
        LOGI_IF(HR_EVENT_DEBUG, "HR Pulse detect");

        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        int64_t pulseTime = (int64_t)ts.tv_sec * 1000*1000*1000 + ts.tv_nsec;
        
        if (mLastPulseTime != 0) {
            int64_t pulseInterval = pulseTime - mLastPulseTime;
            int bpm = (int)((int64_t)60*1000*1000*1000 / pulseInterval);
            
            if (mBpm != 0) {
                mBpm = (mBpm + bpm * 4) / 5; // weighted mean
                mPendingEvent.heart_rate.status = SENSOR_STATUS_ACCURACY_MEDIUM;
                mPendingEvent.heart_rate.bpm = mBpm;
                hasEvent = true;
            } else {
                mBpm = bpm;
            }

            LOGI_IF(HR_EVENT_DEBUG, "HR BPM: %d (%d)", bpm, mBpm);            
        }
        
        mLastPulseTime = pulseTime;
    }
    if (sumChansDetect == 0 && mLastDetect) {
        mLastDetect = false;
    }
    
    return hasEvent;
}

#define S_SAMP_XY_A         0
#define S_SAMP_XY_AB        1
#define S_SAMP_XY_B         2

bool HeartRateSensor::sync() 
{
    int chanSums[2]; // sum of all data channels 
    int chanData[8]; // spectrum channels (0..3: sensor A, 4..7: sensor B)

    if (mSubMode == S_SAMP_XY_A) {
        chanSums[0] = 0;
        chanSums[1] = 0;
        for (int i = 0; i < 4; ++i) {
            chanData[i] = mRawBuffer[i];
            chanData[i+4] = 0;
        }
    } else if (mSubMode == S_SAMP_XY_AB) {
        chanSums[0] = mSumSlotA;
        chanSums[1] = mSumSlotB;
        for (int i = 0; i < 4; ++i) {
            chanData[i] = mRawBuffer[i];
            chanData[i+4] = mRawBuffer[4+i];
        }
    } else if (mSubMode == S_SAMP_XY_B) {
        chanSums[0] = 0;
        chanSums[1] = 0;
        for (int i = 0; i < 4; ++i) {
            chanData[i] = 0;
            chanData[i+4] = mRawBuffer[i];
        }
    }
    
    /*
    LOGI_IF(HR_EVENT_DEBUG, "HR A:[ %d : %d %d %d %d ] B:[ %d : %d %d %d %d ]", 
                chanSums[0], chanData[0], chanData[1], chanData[2], chanData[3],
                chanSums[1], chanData[4], chanData[5], chanData[6], chanData[7]);
    */
    
    return pulseDetect(chanSums, chanData);
}

bool HeartRateSensor::handleEvent(input_event const *event) 
{
    //See kernel drivers/sensors/adpd142.c adpd142_sample_event() for more info
    
    if (event->type == EV_REL) {
        if (event->code == REL_X) {
            mSumSlotA = event->value - 1;
            //LOGI_IF(HR_EVENT_DEBUG, "HR mSumSlotA: %d", mSumSlotA);
        } else if (event->code == REL_Y) {
            mSumSlotB = event->value - 1;
            //LOGI_IF(HR_EVENT_DEBUG, "HR mSumSlotB: %d", mSumSlotB);
        } else if (event->code == REL_Z) {
            mSubMode = event->value - 1;
        }
    } else if (event->type == EV_MSC) {
        if (event->code == MSC_RAW) {
            mRawBuffer[mRawBufferIndex++] = event->value - 0x1;
            //LOGI_IF(HR_EVENT_DEBUG, "HR RAW: %d [%d]", event->value - 0x1, mSubMode);
        }
    } else if (event->type == EV_SYN) {
        //LOGI_IF(HR_EVENT_DEBUG, "HR SYNC");
        mRawBufferIndex = 0;
        return sync();
    } 
    
    return false;
}
