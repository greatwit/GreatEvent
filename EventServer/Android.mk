LOCAL_PATH := $(call my-dir)

LOCAL_PROJECT_ROOT := $(LOCAL_PATH)#$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)))




include $(CLEAR_VARS)

LOCAL_CFLAGS := -D__ANDROID__ -DHAVE_CONFIG_H

LOCAL_MODULE    := netserver

LOCAL_C_INCLUDES += \
				   $(LOCAL_PROJECT_ROOT)/net \
				   $(LOCAL_PROJECT_ROOT)/../common \
				   external/stlport/stlport bionic

LOCAL_SRC_FILES := net/buffer.c \
				net/epoll.c \
				net/epoll_sub.c \
				net/event.c \
				net/evbuffer.c \
				net/signal.c \
				net/log.c \
				spbuffer.cpp \
				sputils.cpp \
				spioutils.cpp \
				spsession.cpp \
				speventcb.cpp \
				spserver.cpp \
				NativeApi.cpp

#LOCAL_STATIC_LIBRARIES := libMediaStream

#LOCAL_SHARED_LIBRARIES := libnativehelper

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

