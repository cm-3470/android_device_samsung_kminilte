# Copyright (C) 2014 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# Modified 2011 by InvenSense, Inc

ifeq ($(TARGET_PROVIDES_LIBSENSORS),true)

LOCAL_PATH := $(call my-dir)

INVENSENSE_PATH := hardware/invensense/6515
INVENSENSE_IIO_PATH := hardware/invensense/6515/libsensors_iio
PROPRIETARY_PATH := vendor/samsung/$(TARGET_DEVICE)/proprietary

# InvenSense fragment of the HAL
include $(CLEAR_VARS)

LOCAL_MODULE := libinvensense_hal
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := invensense

LOCAL_CFLAGS := -DLOG_TAG=\"Sensors\" -Werror -Wall

#
# Invensense uses the OS version to determine whether to include batch support,
# but implemented it in a way that requires modifying the code each time we move
# to a newer OS version.  I will fix this problem in a subsequent change, but for now,
# hardcode to saying we're ANDROID_L so we can isolate this checkin to being
# only changes coming from Invensense.
#
# Setting ANDROID_L to true is perfectly safe even on ANDROID_M because the code
# just requires "ANDROID_L or newer"
#
VERSION_L :=true

ifeq ($(VERSION_KK),true)
LOCAL_CFLAGS += -DANDROID_KITKAT
else
LOCAL_CFLAGS += -DANDROID_LOLLIPOP
endif

LOCAL_CFLAGS += -Wno-error=reorder
LOCAL_CFLAGS += -Wno-error=unused-parameter
LOCAL_CFLAGS += -Wno-error=unused-variable
# for _GYRO_TC_H_ vs _GYRO_TC_H
LOCAL_CFLAGS += -Wno-error=header-guard

LOCAL_SRC_FILES += ../../../../$(INVENSENSE_IIO_PATH)/SensorBase.cpp
LOCAL_SRC_FILES += SamsungSensorBase.cpp
LOCAL_SRC_FILES += MPLSensor.cpp
LOCAL_SRC_FILES += ../../../../$(INVENSENSE_IIO_PATH)/MPLSupport.cpp
LOCAL_SRC_FILES += ../../../../$(INVENSENSE_IIO_PATH)/InputEventReader.cpp
LOCAL_SRC_FILES += CompassSensor.HSCDTD008A.cpp

LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/mllite
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/mllite/linux
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/driver/include
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/driver/include/linux

LOCAL_SHARED_LIBRARIES := liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libmllite

# Additions for SysPed
LOCAL_SHARED_LIBRARIES += libmplmpu
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/mpl
LOCAL_CPPFLAGS += -DLINUX=1

LOCAL_SHARED_LIBRARIES += libmllite
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/mllite
LOCAL_CPPFLAGS += -DLINUX=1

include $(BUILD_SHARED_LIBRARY)


# HAL module implemenation stored in
# hw/<SENSORS_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

LOCAL_MODULE := sensors.universal3470
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

LOCAL_SHARED_LIBRARIES += libmplmpu

LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/mllite
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/mllite/linux
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/mpl
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/driver/include
LOCAL_C_INCLUDES += $(INVENSENSE_IIO_PATH)/software/core/driver/include/linux

LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -DLOG_TAG=\"Sensors\" -Werror -Wall

ifeq ($(VERSION_KK),true)
LOCAL_CFLAGS += -DANDROID_KITKAT
else
LOCAL_CFLAGS += -DANDROID_LOLLIPOP
endif

LOCAL_SRC_FILES := \
	sensors.cpp \
	HeartRateSensor.cpp \
	LightSensor.cpp \
	ProximitySensor.cpp

LOCAL_SHARED_LIBRARIES := libinvensense_hal
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libmllite
LOCAL_SHARED_LIBRARIES += libhardware_legacy
$(info YD>>LOCAL_MODULE=$(LOCAL_MODULE), LOCAL_SRC_FILES=$(LOCAL_SRC_FILES), LOCAL_SHARED_LIBRARIES=$(LOCAL_SHARED_LIBRARIES))
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libmplmpu
LOCAL_SRC_FILES := ../../../../$(PROPRIETARY_PATH)/lib/libmplmpu.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := invensense
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
OVERRIDE_BUILT_MODULE_PATH := $(TARGET_OUT_INTERMEDIATE_LIBRARIES)
LOCAL_STRIP_MODULE := true
include $(BUILD_PREBUILT)

# libmllite contains firmware with SHealth and stepdector support
include $(CLEAR_VARS)
LOCAL_MODULE := libmllite
LOCAL_SRC_FILES := ../../../../$(PROPRIETARY_PATH)/lib/libmllite.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := invensense
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
OVERRIDE_BUILT_MODULE_PATH := $(TARGET_OUT_INTERMEDIATE_LIBRARIES)
LOCAL_STRIP_MODULE := true
include $(BUILD_PREBUILT)

endif
