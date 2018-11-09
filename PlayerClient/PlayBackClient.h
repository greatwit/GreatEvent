#pragma once
#include "NetDataClient.h"
#include"DataBuffer.h"
#include "stdafx.h"
#include "videoclient_interface.h"
class CPlayBackClient : public CNetDataClient
{
public:
	CPlayBackClient(void);
	~CPlayBackClient(void);
	int OpenPlayback(const char* szIP,int nPort,const char* szPath,HWND hWnd,CFILE_INFO* info);
	void CloseClient();
	//DWORD GetCurTime();
	int SetPlayPause(BOOL bPause);
	int Seek(DWORD dwStart);
	//void SetSpeed(int nSpeed);
	//BOOL Capture(const char* szPath);
	int Start(DWORD dwStart,DWORD dwEnd);
	//void SetVol(DWORD dwVol);
	//void SetFramePlay(BOOL bFramePlay);
	int IsPlayerBufferEmpty();
	//void SetOrientation(int nOri);
	//void SetZoom(POINT pt,BOOL bAdd);

private:
	 void DefNetMessage(HANDLE hClient,DWORD dwMsg,DWORD dwIndex,char* lpData,int nLength);
	 BOOL Reconnect();
	 void sendControl();

private:
	//HANDLE m_hPlayer;
	CDataBuffer m_bufData;
	ONRECVAVDATA proc;
	GetEmptyBufferSize getEmpBufSize;
	void* param;
	Buffer m_avBuf;
	BOOL m_bDataEnd;
	//CEvent m_evtSeek;
	DWORD m_dwCurPts;
	CCriticalSection m_csbSend;
	BOOL m_bSend;
	BOOL m_bPause;

};

