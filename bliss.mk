# Inherit some common CM stuff.
$(call inherit-product, vendor/bliss/config/common_full_phone.mk)

# Enhanced NFC
$(call inherit-product, vendor/bliss/config/nfc_enhanced.mk)

$(call inherit-product, device/samsung/kminilte/full_kminilte.mk)

PRODUCT_NAME := bliss_kminilte
