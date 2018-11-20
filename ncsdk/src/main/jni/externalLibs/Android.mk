LOCAL_PATH := $(call my-dir)

#共通ライブラリ
include $(CLEAR_VARS)
LOCAL_MODULE := libcommon
LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/include/common
LOCAL_SRC_FILES := jni/$(TARGET_ARCH_ABI)/$(LOCAL_MODULE)$(TARGET_SONAME_EXTENSION)
include $(PREBUILT_SHARED_LIBRARY)

#aAndusb
include $(CLEAR_VARS)
LOCAL_MODULE := libaandusb
LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/include/aandusb
LOCAL_SRC_FILES := jni/$(TARGET_ARCH_ABI)/$(LOCAL_MODULE)$(TARGET_SONAME_EXTENSION)
include $(PREBUILT_SHARED_LIBRARY)
