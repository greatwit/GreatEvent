####################################
# Build libevent as separate library

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= libevent
LOCAL_MODULE_TAGS:= optional

LOCAL_SRC_FILES := \
    buffer.c \
    epoll.c \
    epoll_sub.c \
    poll.c \
    select.c \
    event.c \
    evbuffer.c \
    signal.c \
    log.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/android \
    $(LOCAL_PATH)/compat \
    external/openssl/include

LOCAL_CFLAGS := -DHAVE_CONFIG_H -DANDROID -fvisibility=hidden

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)
