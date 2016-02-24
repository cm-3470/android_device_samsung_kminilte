#!/bin/bash

CURRENT_DIR=$(dirname $(readlink -f $0))
CM_BASE_DIR=$CURRENT_DIR/../../../..
VENDOR_DIR=$CM_BASE_DIR/vendor/samsung/kminilte/proprietary

# replace_all <old> <new> <file>
# binary string replacement
function replace_all {
    echo "Patching: $(basename $3)"
    perl -pi -e "s/$1/$2/g" $3
}

# KitKat/Lollipop SensorManager is not compatible with Marshmallow, use a compatibility wrapper
replace_all 'SensorManager' 'SensorMgrComp' $VENDOR_DIR/bin/gpsd
# CRYPTO_malloc is now OPENSSL_malloc in BoringSSL which maps to a simple malloc
replace_all 'CRYPTO_malloc' 'malloc\0\0\0\0\0\0\0' $VENDOR_DIR/bin/gpsd

# Do NOT export libsec-ril's Google Protobuffer implementation.
# CM-13 comes with version 2.6.0, libsec-ril with the old (and imcompatible) version 2.3.0.
# When the symbols are exported, gles_trace will crash with a Protobuffer version mismatch 
# error, as it erroneously uses the libsec-ril Protobuffer implementation instead of the CM-13 one.
replace_all 'google8protobuf' 'gxxgle8protobuf' $VENDOR_DIR/lib/libsec-ril.so

# LP camera hal has a reference to libion.so, although it is not using it.
# libexynoscamera.so wants to use ion_alloc()/... of libion_exynos.so but as 
# libion.so (with the functions of the same name) is already referenced,
# the functions of libion.so are erroneously used instead of the libion_exynos.so ones.
# This causes crashes whenever the camera is used (in ion_alloc).
# Solution: replace libion.so with liblog.so which is already referenced by the lib (-> no-op)
replace_all 'libion.so' 'liblog.so' $VENDOR_DIR/lib/hw/camera.universal3470.so

# Signature changed from Lollipop:
#   GraphicBufferMapper::lock(buffer_handle_t handle, int usage, const Rect& bounds, void** vaddr)
#     [_ZN7android19GraphicBufferMapper4lockEPK13native_handleiRKNS_4RectEPPv]
# to Marshmallow:
#   GraphicBufferMapper::lock(buffer_handle_t handle, uint32_t usage, const Rect& bounds, void** vaddr) 
#     [_ZN7android19GraphicBufferMapper4lockEPK13native_handlejRKNS_4RectEPPv]
for lib in $VENDOR_DIR/lib/omx/libOMX.Exynos.*.so; do
    replace_all '_ZN7android19GraphicBufferMapper4lockEPK13native_handleiRKNS_4RectEPPv' \
                '_ZN7android19GraphicBufferMapper4lockEPK13native_handlejRKNS_4RectEPPv' $lib
done
