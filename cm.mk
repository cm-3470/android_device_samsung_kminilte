# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

$(call inherit-product, device/samsung/kminilte/full_kminilte.mk)

PRODUCT_NAME := cm_kminilte
