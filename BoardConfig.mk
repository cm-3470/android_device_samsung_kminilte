#
# Copyright (C) 2014 The CyanogenMod Project
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
#

BOARD_VENDOR := samsung

# Enable dex-preoptimization to speed up first boot sequence
ifeq ($(HOST_OS),linux)
  ifeq ($(TARGET_BUILD_VARIANT),userdebug)
   ifeq ($(WITH_DEXPREOPT),)
    WITH_DEXPREOPT := true
   endif
  endif
endif

# Architecture
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_CPU_VARIANT := cortex-a7
ARCH_ARM_HAVE_TLS_REGISTER := true

# Assert
TARGET_OTA_ASSERT_DEVICE := kminilte

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := universal3470
TARGET_NO_BOOTLOADER := true

# Filesystems
BOARD_BOOTIMAGE_PARTITION_SIZE := 10485760
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 12582912
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2411724800
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 12834570240(-16384)
BOARD_USERDATAIMAGE_PARTITION_SIZE := 12834553856
BOARD_CACHEIMAGE_PARTITION_SIZE := 314572800
BOARD_FLASH_BLOCK_SIZE := 131072

# Include path
TARGET_SPECIFIC_HEADER_PATH := device/samsung/kminilte/include

# Kernel
BOARD_KERNEL_BASE := 0x10000000
BOARD_KERNEL_PAGESIZE := 2048
BOARD_KERNEL_CMDLINE := 
TARGET_KERNEL_CONFIG := kminilte_00_defconfig
TARGET_KERNEL_SOURCE := kernel/samsung/g800f
#TARGET_PREBUILT_KERNEL := device/samsung/kminilte/kernel

# Platform
TARGET_BOARD_PLATFORM := exynos3
TARGET_SOC := exynos3470

# Audio
BOARD_USES_LIBMEDIA_WITH_AUDIOPARAMETER := true

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true
BOARD_BLUEDROID_VENDOR_CONF := device/samsung/kminilte/bluetooth/libbt_vndcfg.txt
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/samsung/kminilte/bluetooth

# Graphics
USE_OPENGL_RENDERER := true
BOARD_EGL_CFG := device/samsung/kminilte/egl/egl.cfg

# Pre-L Compatibility
COMMON_GLOBAL_CFLAGS += \
    -DADD_LEGACY_ACQUIRE_BUFFER_SYMBOL \
    -DENABLE_NON_PIE_SUPPORT

# Legacy MMAP for pre-lollipop blobs
# (needed by mcDriverDaemon which in turn is needed by cbd)
BOARD_USES_LEGACY_MMAP := true

# Media
COMMON_GLOBAL_CFLAGS += -DADD_LEGACY_ACQUIRE_BUFFER_SYMBOL # acquire_buffer symbol for libwvm.so

# RIL
BOARD_PROVIDES_LIBRIL := true
BOARD_MOBILEDATA_INTERFACE_NAME := "rmnet0"
BOARD_MODEM_TYPE := ss222

# Radio
BOARD_RIL_CLASS := ../../../device/samsung/kminilte/ril

# Recovery
BOARD_HAS_NO_SELECT_BUTTON := true

# Webkit
ENABLE_WEBGL := true

# WFD
BOARD_USES_WFD_SERVICE := true

# Wifi
BOARD_WLAN_DEVICE                := bcmdhd
BOARD_HAVE_SAMSUNG_WIFI          := true
BOARD_HOSTAPD_DRIVER             := NL80211
BOARD_HOSTAPD_PRIVATE_LIB        := lib_driver_cmd_${BOARD_WLAN_DEVICE}
BOARD_WPA_SUPPLICANT_DRIVER      := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_${BOARD_WLAN_DEVICE}
WPA_SUPPLICANT_VERSION           := VER_0_8_X
WIFI_DRIVER_FW_PATH_PARAM        := "/sys/module/dhd/parameters/firmware_path"
WIFI_DRIVER_FW_PATH_STA          := "/system/etc/wifi/bcmdhd_sta.bin"
WIFI_DRIVER_FW_PATH_AP           := "/system/etc/wifi/bcmdhd_apsta.bin"
WIFI_DRIVER_FW_PATH_P2P          := "/system/etc/wifi/bcmdhd_p2p.bin"

# Memory allocation
#TARGET_USES_ION := true

# Vendor Init
#TARGET_UNIFIED_DEVICE := true
#TARGET_INIT_VENDOR_LIB := libinit_gardalte
#TARGET_LIBINIT_DEFINES_FILE := device/samsung/gardalte/init/init_gardalte.c

# TWRP
DEVICE_RESOLUTION := 720x1280
BOARD_USE_CUSTOM_RECOVERY_FONT := \"roboto_23x41.h\"
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_RECOVERY_SWIPE := true
BOARD_HAS_NO_MISC_PARTITION := true
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_USES_MMC_UTILS := true
BOARD_SUPPRESS_EMMC_WIPE := true
TW_NO_REBOOT_BOOTLOADER := true
TW_HAS_DOWNLOAD_MODE := true
