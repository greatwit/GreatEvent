LOCAL_PATH := $(call my-dir)

LOCAL_PROJECT_ROOT := $(LOCAL_PATH)#$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)))

FFMPEG_PATH  = ../common/ffmpeg
THREAD_PATH  = ../common/gthread
NAREDEC_PATH = ../common/NalBareflow

include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/android/lib/libavformat.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/android/lib/libavcodec.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/android/lib/libavutil.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/android/lib/libswresample.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

APP_ALLOW_MISSING_DEPS=true

LOCAL_CFLAGS := -D__ANDROID__ -DHAVE_CONFIG_H -DHAV_FFMPEG -D__STDC_CONSTANT_MACROS
LOCAL_CPPFLAGS 	+= -std=c++11
LOCAL_MODULE := netplayer

LOCAL_C_INCLUDES += \
				   $(LOCAL_PROJECT_ROOT)/net \
				   $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH) \
				   $(LOCAL_PROJECT_ROOT)/$(FFMPEG_PATH)/include \
				   $(LOCAL_PROJECT_ROOT)/../common \
				   $(LOCAL_PROJECT_ROOT)/$(THREAD_PATH) \
				   external/stlport/stlport bionic

LOCAL_SRC_FILES := net/buffer.c \
				net/epoll.c \
				net/epoll_sub.c \
				net/event.c \
				net/evbuffer.c \
				net/signal.c \
				net/log.c \
				net/net_protocol.c \
				$(THREAD_PATH)/gthreadpool.cpp \
				$(FFMPEG_PATH)/FfmpegContext.cpp \
				$(NAREDEC_PATH)/NALDecoder.cpp \
				ActorStation.cpp \
				BufferCache.cpp \
				DataUtils.cpp \
				EventCall.cpp \
				IOUtils.cpp \
				Session.cpp \
				TaskBase.cpp \
				TaskFileSend.cpp \
				TaskPlayback.cpp \
				TcpClient.cpp \
				TcpServer.cpp \
				NativeApi.cpp

LOCAL_SHARED_LIBRARIES := avformat avcodec avutil swresample
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

