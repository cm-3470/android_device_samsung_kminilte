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

#include "LightSensor.h"

#define LIGHT_EVENT_DEBUG 0

/*
 * Datasheet CM36686 (Capella Micro) is available under the 
 * alias name VCNL4040M3OE (Vishay).
 */

LightSensor::LightSensor() : 
    SamsungSensorBase(NULL, "light_sensor"),
    mALSData(0),
    mWhiteData(0),
    mLastLux(-1)
{
    mPendingEvent.sensor = ID_L;
    mPendingEvent.type = SENSOR_TYPE_LIGHT;
}

/*
 * The CM36686 kernel driver selects an integration time of 80 ms,
 * resulting in 1/10 lx per step (table 14).
 * It seems that the front glass filters some of the light, so we
 * have to adjust the values.
 */
static float computeLux(int als, int white) 
{
    if ((white <= 4) || (als <= 4)) {
        return 0;
    }

    float scale;
    float alsToWhiteRel = (float)als / (float)white;
    if (alsToWhiteRel >= 0.3) {
        scale = 0.6642 * pow(alsToWhiteRel, 0.1294);
    }
    else {
        scale = 0.4073 * pow(als, 0.0324);
    }
    return scale * als;
}

bool LightSensor::handleEvent(input_event const *event) 
{
    /*
     * See kernel drivers/sensors/cm36686.c for event details.
     * For some reason the driver uses (fake) relative events
     * and adds an offsets for the data of 1.
     */
    
    if (event->type == EV_REL) {
        if (event->code == REL_DIAL) {      
            LOGI_IF(LIGHT_EVENT_DEBUG, "light (als: %d)", event->value);
            mALSData = event->value - 1; // remove offset
        } else if (event->code == REL_WHEEL) {
            LOGI_IF(LIGHT_EVENT_DEBUG, "light (white: %d)", event->value);
            mWhiteData = event->value - 1; // remove offset
        } else if (event->code == REL_MISC) {
            // TODO: does not seem to be used by the kernel driver
            LOGI_IF(LIGHT_EVENT_DEBUG, "light (misc: %d)", event->value);
        }
    } else if (event->type == EV_SYN) {
        // round to full lux to better recognize changes
        int lux = (int)computeLux(mALSData, mWhiteData);
        if (lux != mLastLux) {
            mPendingEvent.light = (float)lux;
            mLastLux = lux;
            LOGI_IF(LIGHT_EVENT_DEBUG, "light (als: %d, white: %d, lux: %f)", 
                    mALSData, mWhiteData, mPendingEvent.light);
            return true;
        }
    }    
    
    // no event created
    return false;
}
