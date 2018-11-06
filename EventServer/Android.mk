LOCAL_PATH := $(call my-dir)

LOCAL_PROJECT_ROOT := $(LOCAL_PATH)#$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)))

include $(CLEAR_VARS)  
LOCAL_MODULE := avformat  
LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/ffmpeg/android/lib/libavformat.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := avcodec  
LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/ffmpeg/android/lib/libavcodec.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := avutil  
LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/ffmpeg/android/lib/libavutil.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := swresample  
LOCAL_SRC_FILES := $(LOCAL_PROJECT_ROOT)/ffmpeg/android/lib/libswresample.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

APP_ALLOW_MISSING_DEPS=true

LOCAL_CFLAGS := -D__ANDROID__ -DHAVE_CONFIG_H -D__STDC_CONSTANT_MACROS

LOCAL_MODULE := netplayer

LOCAL_C_INCLUDES += \
				   $(LOCAL_PROJECT_ROOT)/net \
				   $(LOCAL_PROJECT_ROOT)/ffmpeg \
				   $(LOCAL_PROJECT_ROOT)/ffmpeg/include \
				   $(LOCAL_PROJECT_ROOT)/../common \
				   external/stlport/stlport bionic

LOCAL_SRC_FILES := net/buffer.c \
				net/epoll.c \
				net/epoll_sub.c \
				net/event.c \
				net/evbuffer.c \
				net/signal.c \
				net/log.c \
				net/net_protocol.c \
				ActorStation.cpp \
				BufferCache.cpp \
				DataUtils.cpp \
				EventCall.cpp \
				IOUtils.cpp \
				Session.cpp \
				ffmpeg/FfmpegContext.cpp \
				TaskBase.cpp \
				TaskVideoRecv.cpp \
				TaskVideoSend.cpp \
				TaskPlayback.cpp \
				TcpClient.cpp \
				TcpServer.cpp \
				NALDecoder.cpp \
				NativeApi.cpp

LOCAL_SHARED_LIBRARIES := avformat avcodec avutil swresample
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

