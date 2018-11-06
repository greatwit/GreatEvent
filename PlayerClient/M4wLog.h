#pragma once


#include <stdio.h>

#ifndef TAG
#define TAG "HUADEAN"
#endif
#ifdef __ANDROID__
	#include <android/log.h>
	#define GLOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,TAG,__VA_ARGS__)	//black
	#define GLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,	TAG,  __VA_ARGS__)	//blue
	#define GLOGI(...)  __android_log_print(ANDROID_LOG_INFO,	TAG,  __VA_ARGS__)	//green
	#define GLOGW(...)  __android_log_print(ANDROID_LOG_WARN,	TAG,  __VA_ARGS__)  //yellow
	#define M4W_LOG_ERR(...)  __android_log_print(ANDROID_LOG_ERROR,	TAG,  __VA_ARGS__)  //red
#elif __linux__
#define M4W_LOG_ERR(...) printf("Filename %s, Function %s, Line %d > ", __FILE__, __FUNCTION__, __LINE__); \
                            printf(__VA_ARGS__); \
                            printf("\n");
#define M4W_LOG_DBG(...) printf(__VA_ARGS__); \
							printf("\n");
#define GLOGI GLOGE
#define GLOGW GLOGE
#define GLOGV GLOGE
#endif
