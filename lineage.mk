#
# Copyright 2012 The Android Open Source Project
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

# Get the prebuilt list of APNs
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Inherit from the common Open Source product configuration
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit language packages
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

# A/B updater
AB_OTA_UPDATER := true

AB_OTA_PARTITIONS += \
    boot \
    system

AB_OTA_POSTINSTALL_CONFIG += \
    RUN_POSTINSTALL_system=true \
    POSTINSTALL_PATH_system=system/bin/otapreopt_script \
    FILESYSTEM_TYPE_system=ext4 \
    POSTINSTALL_OPTIONAL_system=true

PRODUCT_PACKAGES += \
    otapreopt_script \
    update_engine \
    update_engine_sideload \
    update_verifier

# The following modules are included in debuggable builds only.
PRODUCT_PACKAGES_DEBUG += \
    bootctl \
    update_engine_client

# Boot control HAL
PRODUCT_PACKAGES += \
    bootctrl.qcom

PRODUCT_STATIC_BOOT_CONTROL_HAL := \
    bootctrl.qcom \
    libgptutils \
    libz

PRODUCT_PACKAGES += \
    charger_res_images \
    charger

# ROM fstab
PRODUCT_COPY_FILES += \
  device/motorola/nash/rootdir/root/fstab.qcom:root/fstab.qcom

# Time Zone data for recovery
PRODUCT_COPY_FILES += \
	system/timezone/output_data/iana/tzdata:recovery/root/system/usr/share/zoneinfo/tzdata

PRODUCT_DEVICE := nash
PRODUCT_NAME := lineage_nash
PRODUCT_BRAND := motorola
PRODUCT_MODEL := Moto Z (2)
PRODUCT_MANUFACTURER := Motorola
