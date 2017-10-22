#
# Copyright (C) 2017 The LineageOS Project
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

# inherit from common smdk3470
-include device/samsung/smdk3470-common/BoardConfigCommon.mk

DEVICE_PATH := device/samsung/kminilte

# Assert
TARGET_OTA_ASSERT_DEVICE := kminiltexx,kminiltedv,kminilteub,kminilte

# Filesystems
# Note: the BOARD_...IMAGE_PARTITION_SIZE values define the image sizes, 
# not the partition sizes. So the image sizes can also be lower than the partition size. 

# Real partition sizes
#BOARD_BOOTIMAGE_PARTITION_SIZE := 10485760
#BOARD_CACHEIMAGE_PARTITION_SIZE := 314572800
#BOARD_RECOVERYIMAGE_PARTITION_SIZE := 12582912
#BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2411724800
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 12834570240(-16384)

# Image sizes
BOARD_BOOTIMAGE_PARTITION_SIZE := 10485760
BOARD_CACHEIMAGE_PARTITION_SIZE := 314572800
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 12582912
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2147483648
BOARD_USERDATAIMAGE_PARTITION_SIZE := 12833521664

# Include path
TARGET_SPECIFIC_HEADER_PATH += $(DEVICE_PATH)/include

# Kernel
TARGET_KERNEL_CONFIG := kminilte_00_defconfig
TARGET_KERNEL_SOURCE := kernel/samsung/kminilte

# Lineage Hardware
BOARD_HARDWARE_CLASS := $(DEVICE_PATH)/lineagehw

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(DEVICE_PATH)/bluetooth

# NFC
# Chipset: Samsung s3fwrn5
# Note: as libnfc-nci only supports pn547 and bcm2079x, select pn547 here but use the stock s3fwrn5 hal
BOARD_NFC_CHIPSET := pn547
BOARD_NFC_HAL_SUFFIX := universal3470

# PowerHAL
TARGET_POWERHAL_VARIANT := exynos3

# Lights
TARGET_PROVIDES_LIBLIGHT := true

# Charger
RED_LED_PATH := "/sys/class/leds/led_r/brightness"
GREEN_LED_PATH := "/sys/class/leds/led_g/brightness"
BLUE_LED_PATH := "/sys/class/leds/led_b/brightness"

# Sensors
TARGET_PROVIDES_LIBSENSORS := true

# GUI
BOARD_EGL_NEEDS_HANDLE_VALUE := true

# Vendor Init
TARGET_INIT_VENDOR_LIB := libinit_kminilte
TARGET_LIBINIT_DEFINES_FILE := $(DEVICE_PATH)/init/init_kminilte.cpp

# Recovery
TARGET_RECOVERY_DEVICE_DIRS += $(DEVICE_PATH)
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/rootdir/etc/fstab.universal3470
BOARD_HAS_LARGE_FILESYSTEM := true

# TWRP (not compatible with user build variant)
#RECOVERY_VARIANT := twrp
DEVICE_RESOLUTION := 720x1280
RECOVERY_SDCARD_ON_DATA := true
BOARD_HAS_NO_REAL_SDCARD := true
TW_BRIGHTNESS_PATH := /sys/devices/platform/s5p-mipi-dsim.0/backlight/panel/brightness
TW_NO_REBOOT_BOOTLOADER := true
TW_HAS_DOWNLOAD_MODE := true
TW_INCLUDE_NTFS_3G := true
TW_INCLUDE_CRYPTO := true
#TW_USE_TOOLBOX := true
TWRP_INCLUDE_LOGCAT := true
TW_NO_LEGACY_PROPS := true

# SELinux
BOARD_SEPOLICY_DIRS := \
	$(DEVICE_PATH)/sepolicy
