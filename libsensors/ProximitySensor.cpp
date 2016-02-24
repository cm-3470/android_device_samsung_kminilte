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
#include <pthread.h>

#include <cutils/log.h>

#include "ProximitySensor.h"

#define PROXIMITY_EVENT_DEBUG 0

/*
 * Datasheet CM36686 (Capella Micro) is available under the 
 * alias name VCNL4040M3OE (Vishay).
 */

ProximitySensor::ProximitySensor() :
    SamsungSensorBase(NULL, "proximity_sensor"),
    mFar(0),
    mLastFar(-1)
{
    mPendingEvent.sensor = ID_PX;
    mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
}

int ProximitySensor::setDelay(int32_t handle, int64_t ns)
{
    return -1;
}

bool ProximitySensor::hasPendingEvents() const 
{
    return mHasPendingEvent;
}

int ProximitySensor::handleEnable(int en) 
{
    if (!en)
        return 0;

    struct input_absinfo absinfo;
    if (!ioctl(data_fd, EVIOCGABS(ABS_DISTANCE), &absinfo)) {
        mHasPendingEvent = true;
        mPendingEvent.distance = indexToValue(absinfo.value);
        return 0;
    } else {
        return -1;
    }
}

float ProximitySensor::indexToValue(size_t index) const
{
    // index: 0 is close, 1 is far
    return index * PROXIMITY_THRESHOLD_CM36686;
}

bool ProximitySensor::handleEvent(input_event const *event) 
{
    /*
     * See kernel drivers/sensors/cm36686.c for event details.
     */
    
    if (event->type == EV_ABS) {
        if (event->code == ABS_DISTANCE) {
            mFar = event->value;
        }
    } else if (event->type == EV_SYN) {
        if (mFar != mLastFar) {
            mPendingEvent.distance = indexToValue(mFar);
            mLastFar = mFar;
            LOGI_IF(PROXIMITY_EVENT_DEBUG, "proximity (dist: %d, %f)", mFar, mPendingEvent.distance);
            return true;
        }
    }    

    // no event created
    return false;
}
