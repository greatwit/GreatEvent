LOCAL_PATH := $(call my-dir)

LOCAL_PROJECT_ROOT := $(LOCAL_PATH)#$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)))




include $(CLEAR_VARS)

LOCAL_CFLAGS := -D__ANDROID__ -DLINUX -DIA32
LOCAL_CPPFLAGS += -fexceptions

#LOCAL_ARM_MODE := arm

LOCAL_MODULE    := netudt

LOCAL_C_INCLUDES += \
			external/stlport/stlport bionic

LOCAL_SRC_FILES := \
		api.cpp \
 		buffer.cpp \
		cache.cpp \
		ccc.cpp \
		channel.cpp \
		common.cpp \
		core.cpp \
		epoll.cpp \
		list.cpp \
		md5.cpp \
		packet.cpp \
		queue.cpp \
		window.cpp \
		NALDecoder.cpp \
		NativeApi.cpp \
		recvfile.cpp \
		sendfile.cpp 
		#recvvideo.cpp \
		#sendvideo.cpp \

#LOCAL_STATIC_LIBRARIES := libMediaStream

#LOCAL_SHARED_LIBRARIES := libnativehelper

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

