#pragma once
#include "videoclient_interface.h"
#include "net/TcpConnect.h"
typedef struct tagWAIT_INFO
{
	CEvent* hWait;
	int   nRetLen;
	char  szWait[1024];
}WAIT_INFO,*LPWAIT_INFO;
class CNetDataClient : public CTcpConnect
{
public:
	CNetDataClient(void);
	virtual ~CNetDataClient(void);

	virtual BOOL Start(DWORD dwStart,DWORD dwEnd);

	virtual BOOL OpenPlayback(const char* szIP,int nPort,const char* szPath,HWND hWnd,CFILE_INFO* lpFileInfo);
	
	virtual void CloseClient();
	virtual DWORD GetCurTime();
	virtual BOOL Capture(const char* szPath);
	virtual void SetSpeed(int nSpeed);
	virtual BOOL SetPlayPause(BOOL bPause);
	virtual BOOL Seek(DWORD dwStart);
	virtual void SetVol(DWORD dwVol);
	virtual void SetFramePlay(BOOL bFramePlay);
	virtual BOOL IsPlayerBufferEmpty();
//	virtual void SetOrientation(int nOri);
//	virtual void SetZoom(POINT pt,BOOL bAdd);
	void StartWork();
protected:
	BOOL OpenClient(const char* szIP,int nPort,const char* szPath,int opt,SFILE_INFO* lpFileInfo);
	
	string ExecCmd(DWORD dwMsg,const char* szMsg,int nMsgLen);
	virtual BOOL OnTimeOut();
private:
	static void* WINAPI __WorkThread(void* param);
	void Work();
protected:
	virtual void DefNetMessage(HANDLE hClient,DWORD dwMsg,DWORD dwIndex,char* lpData,int nLength);

	long GetIndex();

	BOOL SendCmd(DWORD dwCmd,DWORD dwIndex,void* lpData,int nLength);
	BOOL RecvCmd(DWORD& dwCmd,DWORD& dwIndex,void* lpData,int& nLength);
	BOOL RecvData(DWORD& dwCmd,DWORD& dwIndex,void* lpData,int& nLength);
	BOOL waitforQuit();

	virtual BOOL Reconnect();
	
private:
	BOOL ConnectServer(LPSFILE_INFO lpInfo);
protected:
	CEvent m_evtQuit;
	pthread_t m_hThread;
	string m_szFile;
	int m_nTimeOut;
	map<long,LPWAIT_INFO> m_mapWait;
	CCriticalSection m_cs;
private:
	long m_nIndex;
	
	//map<long,LPWAIT_INFO> m_mapWait;
	time_t m_tm;
	CCriticalSection m_csSend;
	FILE			*mwFile;
	
	string m_szIP;
	int m_nPort;
	
	int m_opt;

};

