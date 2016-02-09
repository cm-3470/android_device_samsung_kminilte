#include <errno.h>
#include <cutils/log.h>
#include <string.h>

#include "PressureSensor.dummy.h"

/*****************************************************************************/

PressureSensor::PressureSensor(const char *sysfs_path) 
                  : SensorBase(NULL, NULL),
                    pressure_fd(-1)
{
}

PressureSensor::~PressureSensor()
{
}

int PressureSensor::getFd() const
{
    return pressure_fd;
}

/**
 *  @brief        This function will enable/disable sensor.
 *  @param[in]    handle
 *                  which sensor to enable/disable.
 *  @param[in]    en
 *                  en=1, enable; 
 *                  en=0, disable
 *  @return       if the operation is successful.
 */
int PressureSensor::enable(int32_t handle, int en) 
{
    return -1;
}

int PressureSensor::setDelay(int32_t handle, int64_t ns) 
{
    return 0;
}


/**
    @brief      This function will return the state of the sensor.
    @return     1=enabled; 0=disabled
**/
int PressureSensor::getEnable(int32_t handle)
{
    return 0;
}

/**
 *  @brief  This function will return the current delay for this sensor.
 *  @return delay in nanoseconds. 
 */
int64_t PressureSensor::getDelay(int32_t handle)
{
    return -1;
}

void PressureSensor::fillList(struct sensor_t *list)
{
    list->maxRange = 1100.f;
    list->resolution = 0.0018f;
    list->power = 0.0248f;
    list->minDelay = 100000;
    return;
}
