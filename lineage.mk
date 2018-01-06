#
# Copyright (C) 2017 The LineageOS Open Source Project
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

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from nash device
$(call inherit-product, device/motorola/nash/device.mk)

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

PRODUCT_NAME := lineage_nash
PRODUCT_DEVICE := nash
PRODUCT_MANUFACTURER := Motorola
PRODUCT_BRAND := Motorola
PRODUCT_MODEL := Motorola Z2 Force

PRODUCT_BUILD_PROP_OVERRIDES += \
    BUILD_FINGERPRINT=motorola/nash_tmo_c/nash:7.1.1/NCX26.122-59-8/8:user/release-keys \
    PRIVATE_BUILD_DESC="nash_tmo_c-user 7.1.1 NCX26.122-59-8 8 release-keys"

TARGET_VENDOR := motorola
