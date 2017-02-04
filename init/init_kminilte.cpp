/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>

#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
#include "util.h"

void vendor_load_properties()
{
    std::string platform = property_get("ro.board.platform");
    if (platform != ANDROID_TARGET)
        return;

    std::string bootloader = property_get("ro.bootloader");
    if (bootloader.find("G800F") == 0) {
        /* kminiltexx */
        property_set("ro.build.fingerprint", "samsung/kminiltexx/kminilte:6.0.1/MMB29K/G800FXXU1CPK5:user/release-keys");
        property_set("ro.build.description", "kminiltexx-user 6.0.1 MMB29K G800FXXU1CPK5 release-keys");
        property_set("ro.product.model", "SM-G800F");
        property_set("ro.product.device", "kminiltexx");
    } else if (bootloader.find("G800Y") == 0) {
        /* kminiltedv */
        property_set("ro.build.fingerprint", "samsung/kminiltedv/kminilte:5.1.1/LMY49J/G800YUIS1BPL1:user/release-keys");
        property_set("ro.build.description", "kminiltedv-user 5.1.1 LMY49J G800YUIS1BPL1 release-keys");
        property_set("ro.product.model", "SM-G800Y");
        property_set("ro.product.device", "kminiltedv");
    } else if (bootloader.find("G800M") == 0) {
        /* kminilteub */
        property_set("ro.build.fingerprint", "samsung/kminilteub/kminilte:6.0.1/MMB29K/G800MUBU1CPL1:user/release-keys");
        property_set("ro.build.description", "kminilteub-user 6.0.1 MMB29K G800MUBU1CPL1 release-keys");
        property_set("ro.product.model", "SM-G800M");
        property_set("ro.product.device", "kminilteub");
    }
    
    std::string device = property_get("ro.product.device");
    INFO("Found bootloader id %s setting build properties for %s device\n", bootloader.c_str(), device.c_str());
}
