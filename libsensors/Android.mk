# Copyright (C) 2008 The Android Open Source Project
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

ifeq ($(TARGET_PROVIDES_LIBSENSORS),true)

LOCAL_PATH := $(call my-dir)

INVENSENSE_PATH := hardware/invensense/65xx/libsensors_iio

# InvenSense fragment of the HAL
include $(CLEAR_VARS)

LOCAL_CLANG_CFLAGS += $(my_ignored_clang_warnings)
LOCAL_MODULE := libinvensense_hal
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := invensense

LOCAL_CFLAGS := -DLOG_TAG=\"Sensors\" -Werror -Wall
LOCAL_CFLAGS += -DANDROID_JELLYBEAN

LOCAL_SRC_FILES += ../../../../$(INVENSENSE_PATH)/SensorBase.cpp
LOCAL_SRC_FILES += SamsungSensorBase.cpp
LOCAL_SRC_FILES += MPLSensor.cpp
LOCAL_SRC_FILES += ../../../../$(INVENSENSE_PATH)/MPLSupport.cpp
LOCAL_SRC_FILES += ../../../../$(INVENSENSE_PATH)/InputEventReader.cpp
LOCAL_SRC_FILES += CompassSensor.HSCDTD008A.cpp
LOCAL_SRC_FILES += PressureSensor.dummy.cpp

LOCAL_C_INCLUDES += $(INVENSENSE_PATH)
LOCAL_C_INCLUDES += $(INVENSENSE_PATH)/software/core/mllite
LOCAL_C_INCLUDES += $(INVENSENSE_PATH)/software/core/mllite/linux
LOCAL_C_INCLUDES += $(INVENSENSE_PATH)/software/core/driver/include
LOCAL_C_INCLUDES += $(INVENSENSE_PATH)/software/core/driver/include/linux

LOCAL_SHARED_LIBRARIES := liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libmllite

# Additions for SysPed
LOCAL_SHARED_LIBRARIES += libmplmpu
LOCAL_C_INCLUDES += $(INVENSENSE_PATH)/software/core/mpl
LOCAL_CPPFLAGS += -DLINUX=1

LOCAL_SHARED_LIBRARIES += libmllite
LOCAL_C_INCLUDES += $(INVENSENSE_PATH)/software/core/mllite
LOCAL_CPPFLAGS += -DLINUX=1

include $(BUILD_SHARED_LIBRARY)


# HAL module implemenation stored in
# hw/<SENSORS_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

LOCAL_MODULE := sensors.universal3470
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES += $(INVENSENSE_PATH)

LOCAL_CFLAGS := -DLOG_TAG=\"Sensors\"
LOCAL_CFLAGS += -DANDROID_JELLYBEAN
LOCAL_SRC_FILES := \
	sensors.cpp \
	LightSensor.cpp \
	ProximitySensor.cpp

LOCAL_SHARED_LIBRARIES := libinvensense_hal liblog libcutils libutils libdl

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libmplmpu
LOCAL_SRC_FILES := ../../../../$(INVENSENSE_PATH)/libmplmpu.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := invensense
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
OVERRIDE_BUILT_MODULE_PATH := $(TARGET_OUT_INTERMEDIATE_LIBRARIES)
LOCAL_STRIP_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmllite
LOCAL_SRC_FILES := ../../../../$(INVENSENSE_PATH)/libmllite.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := invensense
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
OVERRIDE_BUILT_MODULE_PATH := $(TARGET_OUT_INTERMEDIATE_LIBRARIES)
LOCAL_STRIP_MODULE := true
include $(BUILD_PREBUILT)

endif