LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := wnpatch
LOCAL_LDLIBS := -llog

# path
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

$(warning  ------------------------------------------------)
$(warning  )
$(warning Author: t.me/weikton)
$(warning Current: $(LOCAL_MODULE).so [$(TARGET_ARCH_ABI)])
$(warning  )
$(warning  ------------------------------------------------)

LOCAL_CPPFLAGS := -w -s -fvisibility=hidden -pthread -Wall -fpack-struct=1 -O2 -std=c++14 -fexceptions
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2
include $(BUILD_SHARED_LIBRARY)