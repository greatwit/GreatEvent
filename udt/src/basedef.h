#pragma once




#include <stdio.h>
#include <stdlib.h>

#ifndef TAG
#define TAG "NetCodec"
#endif

#define  LOGTAG true
#if LOGTAG

#ifdef __ANDROID__
	#include <android/log.h>
	#define GLOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,TAG,  __VA_ARGS__)	//black
	#define GLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,	TAG,  __VA_ARGS__)	//blue
	#define GLOGI(...)  __android_log_print(ANDROID_LOG_INFO,	TAG,  __VA_ARGS__)	//green
	#define GLOGW(...)  __android_log_print(ANDROID_LOG_WARN,	TAG,  __VA_ARGS__)  //yellow
	#define GLOGE(...)  __android_log_print(ANDROID_LOG_ERROR,	TAG,  __VA_ARGS__)  //red
#elif __linux__
#define GLOGD(...) printf("Filename %s, Function %s, Line %d > ", __FILE__, __FUNCTION__, __LINE__); \
                            printf(__VA_ARGS__); \
                            printf("\n");
#define GLOGE(...) printf(__VA_ARGS__); printf("\n");
#define GLOGI(...) printf(__VA_ARGS__); printf("\n");
#define GLOGW(...) printf(__VA_ARGS__); printf("\n");
#define GLOGV(...) printf(__VA_ARGS__); printf("\n");
#endif

#endif

//typedef unsigned short WORD;
//typedef unsigned long DWORD;
//
//#define NET_FLAT	0xfefdfcfb
//
////typedef struct tagNET_CMD
////{
////	DWORD dwFlag;
////	DWORD dwCmd;
////	DWORD dwIndex;
////	DWORD dwLength;
////	char  lpData[];
////}NET_CMD,*LPNET_CMD;
////
////typedef struct tagLOGIN_RET
////{
////	long lRet;
////	int  nLength;
////	char lpData[1];
////}LOGIN_RET,*LPLOGIN_RET;



#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif


#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE	((60* 1024) - 1)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) 	do{delete(p); p = NULL;}while(0)
#endif

#define SAFE_DELETE_ELEMENT(ptr) if(ptr != NULL){ delete ptr; ptr = NULL;}

#ifndef SAFE_FREE
#define SAFE_FREE(p) 	do{free(p); p = NULL;}while(0)
#endif


