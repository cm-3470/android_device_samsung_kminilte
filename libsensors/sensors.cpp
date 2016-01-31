/*
 * Copyright 2013 The Android Open Source Project
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

#define FUNC_LOG LOGV("%s", __PRETTY_FUNCTION__)

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>

#include <linux/input.h>

#include <utils/Atomic.h>
#include <utils/Log.h>

#include "sensors_local.h"
#include "MPLSensor.h"
#include "LightSensor.h"
#include "ProximitySensor.h"
#include "HeartRateSensor.h"

/*****************************************************************************/
/* The SENSORS Module */

#ifdef ENABLE_DMP_SCREEN_AUTO_ROTATION
#define GLOBAL_SENSORS (MPLSensor::NumSensors + 1)
#else
#define GLOBAL_SENSORS MPLSensor::NumSensors
#endif

#ifdef ENABLE_HEARTRATE
#define LOCAL_SENSORS (3)
#else
#define LOCAL_SENSORS (2)
#endif

static struct sensor_t sSensorList[GLOBAL_SENSORS + LOCAL_SENSORS] = {
    {"CM36686 Light Sensor", "CAPELLA", 1, ID_L,
     SENSOR_TYPE_LIGHT, 6553.0f, 0.1f, 0.2f, 100000, 0, 0, 
     SENSOR_STRING_TYPE_LIGHT, "", 0, SENSOR_FLAG_CONTINUOUS_MODE, {}},
     
    {"CM36686 Proximity Sensor", "CAPELLA", 1, ID_PX,
     SENSOR_TYPE_PROXIMITY, 5.0f, 5.0f, 1.3f, 100000, 0, 0,
     SENSOR_STRING_TYPE_PROXIMITY, "", 0, SENSOR_FLAG_CONTINUOUS_MODE, {}},
     
#ifdef ENABLE_HEARTRATE
    {"ADPD142 Heart-Rate Sensor", "Analog Devices", 1, ID_HR,
     SENSOR_TYPE_HEART_RATE, 300.0f, 1.0f, 1.6f, 100000, 0, 0,
     SENSOR_STRING_TYPE_HEART_RATE, "", 0, SENSOR_FLAG_CONTINUOUS_MODE, {}},
#endif
};
static int sensors = LOCAL_SENSORS;

static int open_sensors(const struct hw_module_t* module, const char* id,
                        struct hw_device_t** device);

static int sensors__get_sensors_list(struct sensors_module_t* module,
                                     struct sensor_t const** list)
{
    *list = sSensorList;
    return sensors;
}

static struct hw_module_methods_t sensors_module_methods = {
        open: open_sensors
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: SENSORS_HARDWARE_MODULE_ID,
        name: "Samsung Sensor module",
        author: "Samsung Inc",
        methods: &sensors_module_methods,
        dso: NULL,
        reserved: {0}
    },
    get_sensors_list: sensors__get_sensors_list,
    set_operation_mode: NULL
};

struct sensors_poll_context_t {
    sensors_poll_device_1_t device; // must be first

    sensors_poll_context_t();
    ~sensors_poll_context_t();
    int activate(int handle, int enabled);
    int setDelay(int handle, int64_t ns);
    int pollEvents(sensors_event_t* data, int count);
    int query(int what, int *value);
    int batch(int handle, int flags, int64_t period_ns, int64_t timeout);

    // return true if the constructor is completed
    bool isValid() { return mInitialized; };
    int flush(int handle);

private:
    enum {
        mpl = 0,
        compass,
        dmpOrient,
        dmpSign,
        dmpPed,
        light,
        proximity,
#ifdef ENABLE_HEARTRATE        
        heartrate,
#endif
        numSensorDrivers,   // wake pipe goes here
        numFds,
    };

    struct pollfd mPollFds[numFds];
    SensorBase *mSensor[numSensorDrivers];
    CompassSensor *mCompassSensor;
    // return true if the constructor is completed
    bool mInitialized;

    static const size_t wake = numSensorDrivers;
    static const char WAKE_MESSAGE = 'W';
    int mWritePipeFd;

    int handleToDriver(int handle) const {
        switch (handle) {
            case ID_GY:
            case ID_RG:
            case ID_A:
            case ID_M:
            case ID_RM:
            case ID_PS:
            case ID_O:
            case ID_RV:
            case ID_GRV:
            case ID_LA:
            case ID_GR:
            case ID_SM:
            case ID_P:
            case ID_SC:
            case ID_GMRV:
            case ID_FC:
            case ID_SO:
                return mpl;
            case ID_L:
                return light;
            case ID_PX:
                return proximity;
#ifdef ENABLE_HEARTRATE
            case ID_HR:
                return heartrate;
#endif
        }
        return -EINVAL;
    }
};

/******************************************************************************/

sensors_poll_context_t::sensors_poll_context_t()
{
    VFUNC_LOG;
    mCompassSensor = new CompassSensor();
    MPLSensor *mplSensor = new MPLSensor(mCompassSensor);
    mInitialized = false;
    // Must clean this up early or else the destructor will make a mess.
    memset(mSensor, 0, sizeof(mSensor));

    // setup the callback object for handing mpl callbacks
    setCallbackObject(mplSensor);

    // populate the sensor list
    sensors = LOCAL_SENSORS +
       mplSensor->populateSensorList(&sSensorList[LOCAL_SENSORS],
               sizeof(sSensorList[0]) * (ARRAY_SIZE(sSensorList) - LOCAL_SENSORS));

    mSensor[mpl] = mplSensor;
    mPollFds[mpl].fd = mSensor[mpl]->getFd();
    mPollFds[mpl].events = POLLIN;
    mPollFds[mpl].revents = 0;

    mSensor[compass] = mplSensor;
    mPollFds[compass].fd = mCompassSensor->getFd();
    mPollFds[compass].events = POLLIN;
    mPollFds[compass].revents = 0;

    mSensor[dmpOrient] = mplSensor;
    mPollFds[dmpOrient].fd = ((MPLSensor*) mSensor[dmpOrient])->getDmpOrientFd();
    mPollFds[dmpOrient].events = POLLPRI;
    mPollFds[dmpOrient].revents = 0;

    mSensor[dmpSign] = mplSensor;
    mPollFds[dmpSign].fd = ((MPLSensor*) mSensor[dmpSign])->getDmpSignificantMotionFd();
    mPollFds[dmpSign].events = POLLPRI;
    mPollFds[dmpSign].revents = 0;

    mSensor[dmpPed] = mplSensor;
    mPollFds[dmpPed].fd = ((MPLSensor*) mSensor[dmpPed])->getDmpPedometerFd();
    mPollFds[dmpPed].events = POLLPRI;
    mPollFds[dmpPed].revents = 0;

    mSensor[light] = new LightSensor();
    mPollFds[light].fd = mSensor[light]->getFd();
    mPollFds[light].events = POLLIN;
    mPollFds[light].revents = 0;

    mSensor[proximity] = new ProximitySensor();
    mPollFds[proximity].fd = mSensor[proximity]->getFd();
    mPollFds[proximity].events = POLLIN;
    mPollFds[proximity].revents = 0;

#ifdef ENABLE_HEARTRATE
    mSensor[heartrate] = new HeartRateSensor();
    mPollFds[heartrate].fd = mSensor[heartrate]->getFd();
    mPollFds[heartrate].events = POLLIN;
    mPollFds[heartrate].revents = 0;

    if (mPollFds[heartrate].fd < 0) 
        LOGI("sensors: heart-rate fd is invalid: %d", mPollFds[heartrate].fd);
#endif
    
    if (mPollFds[light].fd < 0 || mPollFds[proximity].fd < 0) 
    {
        LOGE("sensors: fd is invalid: %d, %d", 
             mPollFds[light].fd, mPollFds[proximity].fd);
        delete mCompassSensor;
        return;
    }

    /* Timer based sensor initialization */
    int wakeFds[2];
    int result = pipe(wakeFds);
    LOGE_IF(result<0, "error creating wake pipe (%s)", strerror(errno));
    fcntl(wakeFds[0], F_SETFL, O_NONBLOCK);
    fcntl(wakeFds[1], F_SETFL, O_NONBLOCK);
    mWritePipeFd = wakeFds[1];

    mPollFds[wake].fd = wakeFds[0];
    mPollFds[wake].events = POLLIN;
    mPollFds[wake].revents = 0;
    mInitialized = true;
}

sensors_poll_context_t::~sensors_poll_context_t() {
    FUNC_LOG;
    for (int i = 0 ; i < numSensorDrivers ; i++) {
        delete mSensor[i];
    }
    delete mCompassSensor;
    for (int i = 0; i < numFds; i++) {
        close(mPollFds[i].fd);
    }
    close(mWritePipeFd);
    mInitialized = false;
}

int sensors_poll_context_t::activate(int handle, int enabled) {
    FUNC_LOG;

    int index = handleToDriver(handle);
    
    if (index < 0) return index;
    int err =  mSensor[index]->enable(handle, enabled);
    if (!err) {
        const char wakeMessage(WAKE_MESSAGE);
        int result = write(mWritePipeFd, &wakeMessage, 1);
        LOGE_IF(result < 0, 
                "error sending wake message (%s)", strerror(errno));
    }
    return err;
}

int sensors_poll_context_t::setDelay(int handle, int64_t ns)
{
    FUNC_LOG;
    int index = handleToDriver(handle);
    if (index < 0) return index;
    return mSensor[index]->setDelay(handle, ns);
}

int sensors_poll_context_t::pollEvents(sensors_event_t *data, int count)
{
    VHANDLER_LOG;

    int nbEvents = 0;
    int n = 0;
    int nb, polltime = -1;

    polltime = ((MPLSensor*) mSensor[mpl])->getStepCountPollTime();

    // look for new events
    nb = poll(mPollFds, numSensorDrivers, polltime);
    LOGI_IF(0, "poll nb=%d, count=%d, pt=%d", nb, count, polltime);
    if (nb > 0) {
        for (int i = 0; count && i < numSensorDrivers; i++) {
            SensorBase* const sensor(mSensor[i]);
            if (mPollFds[i].revents & (POLLIN | POLLPRI)) {
                nb = 0;
                if (i == mpl) {
                    ((MPLSensor*) sensor)->buildMpuEvent();
                    mPollFds[i].revents = 0;
                } else if (i == compass) {
                    ((MPLSensor*) sensor)->buildCompassEvent();
                    mPollFds[i].revents = 0;
                } else if (i == dmpOrient) {
                    nb = ((MPLSensor*) sensor)->readDmpOrientEvents(data, count);
                    mPollFds[dmpOrient].revents= 0;
                    if (isDmpScreenAutoRotationEnabled() && nb > 0) {
                        count -= nb;
                        nbEvents += nb;
                        data += nb;
                    }
                } else if (i == dmpSign) {
                    LOGI_IF(0, "HAL: dmpSign interrupt");
                    nb = ((MPLSensor*) sensor)->readDmpSignificantMotionEvents(data, count);
                    mPollFds[i].revents = 0;
                    count -= nb;
                    nbEvents += nb;
                    data += nb;
                } else if (i == dmpPed) {
                    LOGI_IF(0, "HAL: dmpPed interrupt");
                    nb = ((MPLSensor*) sensor)->readDmpPedometerEvents(data, count, ID_P, SENSOR_TYPE_STEP_DETECTOR, 0);
                    mPollFds[i].revents = 0;
                    count -= nb;
                    nbEvents += nb;
                    data += nb;
                } else if (i == light || i == proximity) {
                    LOGI_IF(0, "HAL: Light interrupt");
                    nb = sensor->readEvents(data, count);
                    if (nb < count) {
                        // no more data for this sensor
                        mPollFds[i].revents = 0;
                    }
                    count -= nb;
                    nbEvents += nb;
                    data += nb;
                }
                #if 1
                if(nb == 0) {
                    nb = sensor->readEvents(data, count);
                    LOGI_IF(0, "sensors_mpl:readEvents() - "
                            "i=%d, nb=%d, count=%d, nbEvents=%d, "
                            "data->timestamp=%lld, data->data[0]=%f,",
                            i, nb, count, nbEvents, data->timestamp, 
                            data->data[0]);
                    if (nb > 0) {
                        count -= nb;
                        nbEvents += nb;
                        data += nb;
                    }
                }
                #endif
            }
        }

        /* to see if any step counter events */
        if (((MPLSensor*) mSensor[mpl])->hasStepCountPendingEvents() == true) {
            nb = 0;
            nb = ((MPLSensor*) mSensor[mpl])->readDmpPedometerEvents(data, count, ID_SC, SENSOR_TYPE_STEP_COUNTER, 0);
            LOGI_IF(SensorBase::HANDLER_DATA, "sensors_mpl:readStepCount() - "
                    "nb=%d, count=%d, nbEvents=%d, data->timestamp=%lld, "
                    "data->data[0]=%f,",
                    nb, count, nbEvents, data->timestamp, data->data[0]);
            if (nb > 0) {
                count -= nb;
                nbEvents += nb;
                data += nb;
            }
        }
    } else if(nb == 0) {
        /* to see if any step counter events */
        if(((MPLSensor*) mSensor[mpl])->hasStepCountPendingEvents() == true) {
            nb = 0;
            nb = ((MPLSensor*) mSensor[mpl])->readDmpPedometerEvents(
                            data, count, ID_SC, SENSOR_TYPE_STEP_COUNTER, 0);
            LOGI_IF(SensorBase::HANDLER_DATA, "sensors_mpl:readStepCount() - "
                    "nb=%d, count=%d, nbEvents=%d, data->timestamp=%lld, "
                    "data->data[0]=%f,",
                    nb, count, nbEvents, data->timestamp, data->data[0]);
            if (nb > 0) {
                count -= nb;
                nbEvents += nb;
                data += nb;
            }
        }

        if (mPollFds[wake].revents & POLLIN) {
            char msg;
            int result = read(mPollFds[wake].fd, &msg, 1);
            LOGE_IF(result < 0, 
                    "error reading from wake pipe (%s)", strerror(errno));
            mPollFds[numSensorDrivers].revents = 0;
        }
    }
    return nbEvents;
}

int sensors_poll_context_t::query(int what, int* value)
{
    FUNC_LOG;
    return ((MPLSensor*) mSensor[mpl])->query(what, value);
}

int sensors_poll_context_t::batch(int handle, int flags, int64_t period_ns, int64_t timeout)
{
    FUNC_LOG;
    int index = handleToDriver(handle);
    if (index < 0) return index;
    return mSensor[index]->batch(handle, flags, period_ns, timeout);
}

int sensors_poll_context_t::flush(int handle)
{
    FUNC_LOG;
    int index = handleToDriver(handle);
    if (index < 0) return index;
    return mSensor[index]->flush(handle);
}

/******************************************************************************/

static int poll__close(struct hw_device_t *dev)
{
    FUNC_LOG;
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    if (ctx) {
        delete ctx;
    }
    return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev,
                          int handle, int enabled)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->activate(handle, enabled);
}

static int poll__setDelay(struct sensors_poll_device_t *dev,
                          int handle, int64_t ns)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    int s= ctx->setDelay(handle, ns);
    return s;
}

static int poll__poll(struct sensors_poll_device_t *dev,
                      sensors_event_t* data, int count)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->pollEvents(data, count);
}

static int poll__batch(struct sensors_poll_device_1 *dev,
                      int handle, int flags, int64_t period_ns, int64_t timeout)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->batch(handle, flags, period_ns, timeout);
}

static int poll__flush(struct sensors_poll_device_1 *dev,
                      int handle)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->flush(handle);
}
/******************************************************************************/

/** Open a new instance of a sensor device using name */
static int open_sensors(const struct hw_module_t* module, const char* id,
                        struct hw_device_t** device)
{
    FUNC_LOG;
    int status = -EINVAL;
    sensors_poll_context_t *dev = new sensors_poll_context_t();

    if (!dev->isValid()) {
        ALOGE("Failed to open the sensors");
        return status;
    }
    
    memset(&dev->device, 0, sizeof(sensors_poll_device_1));

    dev->device.common.tag = HARDWARE_DEVICE_TAG;
    dev->device.common.version  = SENSORS_DEVICE_API_VERSION_1_0;
    dev->device.common.module   = const_cast<hw_module_t*>(module);
    dev->device.common.close    = poll__close;
    dev->device.activate        = poll__activate;
    dev->device.setDelay        = poll__setDelay;
    dev->device.poll            = poll__poll;

    /* Batch processing */
    dev->device.batch           = poll__batch;
    dev->device.flush           = poll__flush;

    *device = &dev->device.common;
    status = 0;

    return status;
}
