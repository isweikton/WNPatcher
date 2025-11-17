#
# Created by weikton on 17.11.25.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := WnPatcher
LOCAL_SRC_FILES := main.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_CFLAGS := -w -s -pthread -fpack-struct=1 -O2 -std=c++11 -fexceptions -frtti -fstrict-aliasing -fno-omit-frame-pointer -fstack-protector -fno-short-enums -fvisibility=default
LOCAL_CPPFLAGS := $(LOCAL_CFLAGS)
LOCAL_CFLAGS += -DRAKSAMP_CLIENT
LOCAL_LDLIBS := -llog -lz -landroid

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_CFLAGS += -DIS_ARM
else ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_CFLAGS += -DIS_ARM64
endif

LOCAL_STATIC_LIBRARIES := glosshook

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := glosshook
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libGlossHook.a
include $(PREBUILT_STATIC_LIBRARY)