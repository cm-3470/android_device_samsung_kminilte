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

#ifndef ANDROID_LIGHT_SENSOR_H
#define ANDROID_LIGHT_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "sensors_local.h"
#include "SamsungSensorBase.h"
#include "InputEventReader.h"

/*****************************************************************************/

struct input_event;

class LightSensor: public SamsungSensorBase {
public:
    LightSensor();
    
    virtual bool handleEvent(input_event const * event);
    
private:
    /* Ambient Light Sensor (ALS) channel data */
    int mALSData;
    /* White channel data */
    int mWhiteData;
    /* last reported lux value (-1 if uninitialized)*/
    int mLastLux;
};

/*****************************************************************************/

#endif  /* ANDROID_LIGHT_SENSOR_H */
