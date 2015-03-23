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

# inherit from common smdk3470
-include device/samsung/smdk3470-common/BoardConfigCommon.mk

# Enable dex-preoptimization to speed up first boot sequence
ifeq ($(HOST_OS),linux)
  ifeq ($(TARGET_BUILD_VARIANT),userdebug)
   ifeq ($(WITH_DEXPREOPT),)
    WITH_DEXPREOPT := true
   endif
  endif
endif

# Assert
TARGET_OTA_ASSERT_DEVICE := kminilte

# Filesystems
BOARD_BOOTIMAGE_PARTITION_SIZE := 10485760
BOARD_CACHEIMAGE_PARTITION_SIZE := 314572800
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 12582912
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2411724800
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 12834570240(-16384)
BOARD_USERDATAIMAGE_PARTITION_SIZE := 12834553856

# Include path
TARGET_SPECIFIC_HEADER_PATH += device/samsung/kminilte/include

# Kernel
TARGET_KERNEL_CONFIG := kminilte_00_defconfig
TARGET_KERNEL_SOURCE := kernel/samsung/kminilte

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/samsung/kminilte/bluetooth

# NFC
# Chipset: Samsung s3fwrn5
# Note: as libnfc-nci only supports pn547 and bcm2079x, select pn547 here but use the stock s3fwrn5 hal
BOARD_NFC_CHIPSET := pn547
BOARD_NFC_HAL_SUFFIX := universal3470

# Recovery
TARGET_RECOVERY_DEVICE_DIRS += device/samsung/kminilte
TARGET_RECOVERY_FSTAB := device/samsung/kminilte/rootdir/etc/fstab.universal3470

# SELinux
BOARD_SEPOLICY_DIRS += \
   device/samsung/kminilte/sepolicy

BOARD_SEPOLICY_UNION += \
    file_contexts \
    service_contexts \
    device.te \
    domain.te \
    drmserver.te \
    file.te \
    gpsd.te \
    init.te \
    mediaserver.te \
    servicemanager.te \
    system_app.te \
    system_server.te \
    vold.te \
    wpa.te

# TWRP
DEVICE_RESOLUTION := 720x1280
RECOVERY_SDCARD_ON_DATA := true
BOARD_HAS_NO_REAL_SDCARD := true
TW_NO_REBOOT_BOOTLOADER := true
TW_HAS_DOWNLOAD_MODE := true
