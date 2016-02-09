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

#ifndef ANDROID_HEARTRATE_SENSOR_H
#define ANDROID_HEARTRATE_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "sensors_local.h"
#include "SamsungSensorBase.h"
#include "InputEventReader.h"

/*****************************************************************************/

struct input_event;

class HeartRateSensor: public SamsungSensorBase 
{
public:
    HeartRateSensor();
    ~HeartRateSensor();
    
    virtual int enable(int32_t handle, int en);
    virtual bool handleEvent(input_event const * event);

private:
    
    bool sync();
    
    void pulseDetectReset();
    bool pulseDetect(int *chanSums, int *chanData);
    
private:

    int mRawBuffer[8];
    int mRawBufferIndex;
    int mSumSlotA;
    int mSumSlotB;
    int mSubMode;
    
private:

    int mAvg[8];
    int mMinVal[8];
    int mMaxVal[8];
    bool mLastDetect;
    int mBpm;
    int64_t mLastPulseTime;
};

/*****************************************************************************/

#endif  /* ANDROID_HEARTRATE_SENSOR_H */
