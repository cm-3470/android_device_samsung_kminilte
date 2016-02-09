#ifndef SENSORS_LOCAL_H
#define SENSORS_LOCAL_H

#include "sensors.h"

/* additional IDs */
enum {
    ID_L = 0x100,
    ID_PX,
    ID_HR
};

// EVENT_TYPE_MAGV_n already defined in sensors.h
// -> Use EVENT_TYPE_REL_MAGV_n instead.
#define EVENT_TYPE_REL_MAGV_X           REL_X
#define EVENT_TYPE_REL_MAGV_Y           REL_Y
#define EVENT_TYPE_REL_MAGV_Z           REL_Z

#endif  // SENSORS_LOCAL_H
