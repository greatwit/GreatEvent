#pragma once

#include "stdafx.h"


//typedef void (CALLBACK *fDrawFun)(HDC hDC,DWORD dwUser,RECT rect,int nWidth,int nLength);
typedef struct tagPLABACK_FILE_INFO
{
//	DWORD tmStart;
//	DWORD tmEnd;
	union
	{
		ONDOWNLAODSTATUS proc;
	//	fDrawFun procDraw;
	};
	void* param;
}PLABACK_FILE_INFO,*LPPLABACK_FILE_INFO;


/*typedef void (CALLBACK* DEFNETMESSAGE)(HANDLE hClient,DWORD dwMsg,DWORD dwIndex,char* lpData,int nLength,void* param);
typedef void (CALLBACK *ONDOWNLAODSTATUS)(HANDLE hDownload,const char* szSrcFile,const char* szDstFile,DWORD dwPos,DWORD dwTotal,void* param);
typedef void (CALLBACK* ONRECVAVDATA)(void* param,LPAV_FRAME lpFrame);*/
typedef struct tagLOGIN_INFO
{
	char szLoginParam[1024];
	int   nParamLen;
	DEFNETMESSAGE proc;
	void* procParam;
	BOOL  bPing;
	char* szRet;
	int*  nRetLength;
}LOGIN_INFO,*LPLOGIN_INFO;

#ifndef DLL_EXPORT
#define DLL_EXPORT
#endif

DLL_EXPORT HANDLE WINAPI VIDEOCLIENT_OpenPlayback(const char* szIP,int nPort,const char* szPath,HWND hWnd,CFILE_INFO* lpFileInfo);
DLL_EXPORT HANDLE WINAPI VIDEOCLIENT_DownloadFile(const char* szIP,int nPort,const char* szSrcFile,const char* szSavePath,LPPLABACK_FILE_INFO lpFileInfo);
DLL_EXPORT void WINAPI VIDEOCLIENT_CloseDownLoad(HANDLE hDownload);
DLL_EXPORT BOOL WINAPI VIDEOCLIENT_Start(HANDLE hClient,DWORD dwStart,DWORD dwEnd);
DLL_EXPORT void WINAPI VIDEOCLIENT_StopPlayback(HANDLE hClient);
DLL_EXPORT DWORD WINAPI VIDEOCLIENT_GetCurTime(HANDLE hClient);
DLL_EXPORT int WINAPI VIDEOCLIENT_SetPlayPause(HANDLE hClient,BOOL bPause);
DLL_EXPORT BOOL WINAPI VIDEOCLIENT_Seek(HANDLE hClient,DWORD dwPos);
DLL_EXPORT void WINAPI VIDEOCLIENT_SetSpeed(HANDLE hClient,int nSpeed);
DLL_EXPORT BOOL WINAPI VIDEOCLIENT_Capture(HANDLE hClient,const char* szPath);
DLL_EXPORT void WINAPI VIDEOCLIENT_SetVol(HANDLE hClient,DWORD dwVol);
DLL_EXPORT void  WINAPI VIDEOCLIENT_SetFramePlay(HANDLE hClient,BOOL bFramePlay);
DLL_EXPORT DWORD  WINAPI VIDEOCLIENT_GetPlayStatus(HANDLE hClient);
DLL_EXPORT void WINAPI VIDEOCLIENT_SetOrientation(HANDLE hClient,int nOri);
//DLL_EXPORT void WINAPI VIDEOCLIENT_SetZoom(HANDLE hClient,POINT pt,BOOL bAdd);
DLL_EXPORT BOOL  WINAPI VIDEOCLIENT_IsDataEmpty(HANDLE hClient);






//int mytest(const char *argv);
//int audresp(unsigned char* srcDat,int len,int srcCh,int srcBit,int srcRate,unsigned char** dst_data);


///////////////////////////add by hxj 2017-1-12///////////////////////////////////////////////


#ifndef DRAW_FUN
#define DRAW_FUN
//typedef void (CALLBACK* fDrawFun)(HDC hDC,DWORD dwUser,RECT rect,int nWidth,int nHeight);
#endif



typedef void(CALLBACK* OnTalkData)(HANDLE hTalk,char* lpData,int nLength,void* param);

typedef struct tagOPEN_INFO
{
	HWND hWnd;
	DWORD dwSid;
	DWORD dwDevID;
	DWORD dwChannel;
	DWORD dwChIndex;
	char szText[256];
	/*fDrawFun proc;
	DWORD dwUser;*/
	char  szUser[20];
	char  szPwd[20];
}OPEN_INFO,*LPOPEN_INFO;

typedef struct tagOPEN_TALK
{
	DWORD dwSid;
	DWORD dwDevID;

	//	OnTalkData proc;
	//	void* param;
	char  szUser[20];
	char  szPwd[20];
}OPEN_TALK,*LPOPEN_TALK;

#ifndef DLL_EXPORT
#define DLL_EXPORT
#endif

DLL_EXPORT void VIDEOCLIENT_SetRecordPath(const char* szPath);
DLL_EXPORT BOOL VIDEOCLIENT_IsRecordDiskFull();

DLL_EXPORT HANDLE VIDEOCLIENT_OpenVideo(const char* szIP,int nPort,LPOPEN_INFO lpInfo);
DLL_EXPORT void VIDEOCLIENT_CloseVideo(HANDLE hVideoClient);
//DLL_EXPORT BOOL VIDEOCLIENT_Capture(HANDLE hVideoClient,const char* szPath);
DLL_EXPORT float VIDEOCLIENT_GetRate(HANDLE hVideoClient);
DLL_EXPORT void VIDEOCLIENT_SetRecord(HANDLE hClient,BOOL bRecord);
DLL_EXPORT BOOL VIDEOCLIENT_IsRecord(HANDLE hClient);
DLL_EXPORT int VIDEOCLIENT_GetVolume(HANDLE hClient);
DLL_EXPORT void VIDEOCLIENT_SetVolume(HANDLE hClient,int nVol);
DLL_EXPORT void VIDEOCLIENT_SetAudioPause(HANDLE hClient,BOOL bPause);

DLL_EXPORT HANDLE VIDEOCLIENT_OpenTalk(const char* szIP,int nPort,LPOPEN_TALK lpInfo);
DLL_EXPORT void VIDEOCLIENT_CloseTalk(HANDLE hTalk);

DLL_EXPORT int VIDEOCLIENT_GetTalkVolume(HANDLE hTalk);
DLL_EXPORT void VIDEOCLIENT_SetTalkVolume(HANDLE hTalk,int nVol);

DLL_EXPORT void VIDEOCLIENT_Uninit();

//针对报警录像设计的接口 add by hxj at 2017-4-1
DLL_EXPORT HANDLE VIDEOCLIENT_AlarmRecordOpenVideo(const char* szIP,int nPort,LPOPEN_INFO lpInfo);
DLL_EXPORT void VIDEOCLIENT_AlarmRecordCloseVideo(HANDLE hVideoClient);
DLL_EXPORT void VIDEOCLIENT_AlarmRecordSetRecord(HANDLE hClient,BOOL bRecord);
DLL_EXPORT BOOL VIDEOCLIENT_AlarmRecordIsRecord(HANDLE hClient);
//modiby by hxj at 2017-4-13
DLL_EXPORT void VIDEOCLIENT_AlarmRecord2CloseVideo(HANDLE hVideoClient);
DLL_EXPORT HANDLE VIDEOCLIENT_AlarmRecord2OpenVideo(const char* szIP,int nPort,DWORD dwSid,DWORD dwDevID,DWORD dwChannel,DWORD dwChIndex,const char *m_szText);
DLL_EXPORT BOOL VIDEOCLIENT_OpenVideoTalk(HANDLE hClient);
DLL_EXPORT BOOL VIDEOCLIENT_CloseVideoTalk(HANDLE hClient);
DLL_EXPORT BOOL VIDEOCLIENT_IsVideoTalk(HANDLE hClient);
