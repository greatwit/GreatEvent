LOCAL_PATH := $(call my-dir)

LOCAL_PROJECT_ROOT := $(LOCAL_PATH)#$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)))


THREAD_PATH  = ../common/gthread

include $(CLEAR_VARS)

LOCAL_MODULE    := netcodec

LOCAL_C_INCLUDES += \
				   $(LOCAL_PROJECT_ROOT)/$(THREAD_PATH) \
				   $(LOCAL_PROJECT_ROOT)/includes \
				   $(LOCAL_PROJECT_ROOT)/../common \
				   external/stlport/stlport bionic

LOCAL_SRC_FILES := NALDecoder.cpp \
				   CodecNative.cpp \
				   $(THREAD_PATH)/gmutex.cpp \
				   $(THREAD_PATH)/gthread.cpp \
				   spioutils.cpp \
				   TcpSender.cpp

#LOCAL_STATIC_LIBRARIES := libMediaStream
#LOCAL_SHARED_LIBRARIES := libnativehelper

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

