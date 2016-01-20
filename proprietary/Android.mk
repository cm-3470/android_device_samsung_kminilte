LOCAL_PATH := $(call my-dir)

#NOTE: To use this, TARGET_EXYNOS3_AUDIO_FROM_SOURCE must be set to false in the boardconfig!

ifneq ($(filter false,$(TARGET_EXYNOS3_AUDIO_FROM_SOURCE)),)

VENDOR_OUT := out/target/product/kminilte/system

include $(CLEAR_VARS)
LOCAL_MODULE		:= audio.vendor.universal3470
LOCAL_MODULE_TAGS	:= optional
LOCAL_MODULE_SUFFIX 	:= .so
LOCAL_SRC_FILES		:= audio.vendor.universal3470.so
LOCAL_MODULE_CLASS 	:= SHARED_LIBRARIES
LOCAL_MODULE_PATH	:= $(VENDOR_OUT)/lib/hw
include $(BUILD_PREBUILT)

endif
