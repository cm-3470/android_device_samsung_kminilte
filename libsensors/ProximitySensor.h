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

#ifndef ANDROID_PROXIMITY_SENSOR_H
#define ANDROID_PROXIMITY_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "sensors_local.h"
#include "SamsungSensorBase.h"
#include "InputEventReader.h"

/* the CM36686 is a binary proximity sensor that triggers around 5 cm on
 * this hardware */
#define PROXIMITY_THRESHOLD_CM36686  5.0f

/*****************************************************************************/

struct input_event;

class ProximitySensor:public SamsungSensorBase {

    virtual int handleEnable(int en);
    virtual bool handleEvent(input_event const * event);

    float indexToValue(size_t index) const;
public:
    ProximitySensor();
    virtual int setDelay(int32_t handle, int64_t ns);
    virtual bool hasPendingEvents() const;
private:
    int mFar; // 0: close, 1: far
};

/*****************************************************************************/

#endif  /* ANDROID_PROXIMITY_SENSOR_H */
