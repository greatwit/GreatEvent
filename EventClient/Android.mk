LOCAL_PATH := $(call my-dir)

LOCAL_PROJECT_ROOT := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)))




include $(CLEAR_VARS)

LOCAL_MODULE    := netcodec

LOCAL_C_INCLUDES += \
				   $(LOCAL_PROJECT_ROOT)/gthread \
				   $(LOCAL_PROJECT_ROOT)/includes \
				   external/stlport/stlport bionic

LOCAL_SRC_FILES := NALDecoder.cpp \
				   CodecNative.cpp \
				   gthread/gmutex.cpp \
				   gthread/gthread.cpp \
				   spioutils.cpp \
				   RtpSender.cpp \
				   H264Sender.cpp \
				   TcpSender.cpp

#LOCAL_STATIC_LIBRARIES := libMediaStream

#LOCAL_SHARED_LIBRARIES := libnativehelper

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
