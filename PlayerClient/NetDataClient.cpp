#include "NetDataClient.h"
#include <stdlib.h>

#include <unistd.h>

#ifdef 	__ANDROID__
#define	FILE_PATH	"/sdcard/w.h264"
#else
#define	FILE_PATH	"w.h264"
#endif

CNetDataClient::CNetDataClient(void) :m_evtQuit()
{

	m_nIndex = 0;
	m_tm = 0;
	m_hThread = 0;
//	m_hDwnlThread = NULL;
	m_nPort = 0;
	m_opt = 0;
	m_nTimeOut = 15;

	mwFile = fopen(FILE_PATH, "wb+");

	//add at 2017-1-13
	/*m_nPort = 0;
	m_tm = 0;
	m_lpProc = NULL;
	m_lpParam = NULL;
	m_loginParam = NULL;
	m_nLoginLen = 0;
	m_bPing = FALSE;
	m_hThread = NULL;*/
}


CNetDataClient::~CNetDataClient(void)
{
	CloseClient();

	if(mwFile != NULL)
		fclose(mwFile);

	//add 2017-1-18
	//Logoff();
}
void CNetDataClient::SetFramePlay(BOOL bFramePlay)
{
}
void CNetDataClient::SetVol(DWORD dwVol)
{
}
long CNetDataClient::GetIndex()
{
	pthread_mutex_t t;
	pthread_mutex_init (&t,NULL);
	//pthread_mutex_t t=PTHREAD_MUTEX_DEFAULT;
	pthread_mutex_lock(&t);
	do{
		m_nIndex++;
	}while(m_nIndex == 0);
	pthread_mutex_unlock(&t);
	return m_nIndex;
}
BOOL CNetDataClient::SendCmd(DWORD dwCmd,DWORD dwIndex,void* lpData,int nLength)
{
	CSingleLock sLock(&m_csSend,TRUE);
	m_tm = time(NULL);
	NET_CMD nc;
	memset(&nc,0,sizeof(nc));
	nc.dwFlag = NET_FLAG;
	nc.dwCmd = dwCmd;
	nc.dwIndex = dwIndex;
	nc.dwLength = nLength;
	if (!Send(&nc,sizeof(nc)))
	{
		M4W_LOG_ERR("send nc err! len = %d",nLength);
		return FALSE;
	}
	M4W_LOG_DBG("send head len:%d ", sizeof(nc));
	if (nLength == 0)
	{
		return TRUE;
	}	
	if(!Send((void*)lpData,nLength))
	{
		M4W_LOG_ERR("send lpdata err len = %d",nLength);
		return FALSE;
	}
	M4W_LOG_DBG("send data len:%d lpData:%s", nLength, lpData);
	return TRUE;

	
}

BOOL CNetDataClient::RecvCmd(DWORD& dwCmd,DWORD& dwIndex,void* lpLogin,int& nLength)
{
	NET_CMD nc;
	if (!Recv(&nc,sizeof(NET_CMD)))
	{
		M4W_LOG_ERR("recv err 111");
		return FALSE;
	}
	if (nc.dwFlag != NET_FLAG)
	{
		M4W_LOG_ERR("recv err 222");
		return FALSE;
	}
	if (nc.dwLength == 0)
	{
		nLength = 0;
		dwCmd = nc.dwCmd;
		dwIndex = nc.dwIndex;
		return TRUE;
	}

	if (nc.dwLength > nLength)
	{
		M4W_LOG_ERR("recv err 333");
		return FALSE;
	}
	if (!Recv(lpLogin,nc.dwLength))
	{
		M4W_LOG_ERR("recv err 4444");
		return FALSE;
	}
	M4W_LOG_ERR("recv dwlen=%d",nc.dwLength);

	LPLOGIN_RET lpRet = (LPLOGIN_RET)lpLogin;
	printf("LPLOGIN_RET len:%d\n", lpRet->nLength);



	LPSFILE_INFO	  fileInfo = (LPSFILE_INFO)lpRet->lpData;
	PLAYER_INIT_INFO &playInfo = fileInfo->pi;
	printf("w:%d h:%d size:%d framerate:%d\n",
			playInfo.nWidth, playInfo.nHeigth,
			playInfo.gop_size, playInfo.nFps);

	printf("nAudioFormat:%d nChannel:%d nSampleRate:%d bit_rate:%d\n",
			playInfo.nAudioFormat, playInfo.nChannel,
			playInfo.nSampleRate, playInfo.bit_rate);


	nLength = nc.dwLength;
	dwCmd = nc.dwCmd;
	dwIndex = nc.dwIndex;
	return TRUE;
}

BOOL CNetDataClient::RecvData(DWORD& dwCmd,DWORD& dwIndex,void* lpData,int& nLength) {
	//int res = 0;
	static int count = 0;
	NET_CMD nc;
	if (!Recv(&nc,sizeof(NET_CMD)))
	{
		M4W_LOG_ERR("recv err 111");
		return FALSE;
	}
	if (nc.dwFlag != NET_FLAG)
	{
		M4W_LOG_ERR("recv err 222");
		return FALSE;
	}
//	if (nc.dwLength == 0)
//	{
//		nLength = 0;
//		dwCmd = nc.dwCmd;
//		dwIndex = nc.dwIndex;
//		return TRUE;
//	}

	if (nc.dwLength > nLength)
	{
		M4W_LOG_ERR("recv err 333");
		return FALSE;
	}

	M4W_LOG_ERR("recv cmd=%d",nc.dwCmd);
	switch(nc.dwCmd) {

	case MODULE_MSG_VIDEO:
		LPAV_FRAME av;
		if (!Recv(lpData, nc.dwLength))
		{
			M4W_LOG_ERR("recv err 4444");
			return FALSE;
		}
		av = (LPAV_FRAME)lpData;
		if(av->dwFrameType==1 || av->dwFrameType==2) {
			count++;
			//char tag[4] = {0x00, 0x00, 0x00, 0x01};
			//fwrite(tag, 1, 4, mwFile);
			fwrite(av->lpData, 1, av->nLength, mwFile);
			//M4W_LOG_ERR("recv avlen=%d type:%d count:%d",av->nLength, av->dwFrameType, count);
		}
		break;

	case MODULE_MSG_DATAEND:
		return FALSE;
	}


	//usleep(10*1000);
	return TRUE;
}

BOOL CNetDataClient::ConnectServer(SFILE_INFO* lpInfo)
{
	if(!Connect(m_szIP.c_str(),m_nPort))
		return FALSE;

	M4W_LOG_ERR("connect suc opt = %d",m_opt);
	char lpData[2048];
	int nLength = 0;
	if(m_opt == 0)
	{
		nLength = sprintf(lpData,"<play path=\"%s\"/>",m_szFile.c_str());
	} else {
		nLength = sprintf(lpData,"<get path=\"%s\"/>",m_szFile.c_str());
	}
	
	if(!SendCmd(MODULE_MSG_LOGIN,0,lpData,nLength))
	{
		M4W_LOG_ERR("send CMD err!");
		return FALSE;
	}


	nLength = 2048;
	DWORD dwMsg;
	DWORD dwIndex;
	if (!RecvCmd(dwMsg,dwIndex,lpData,nLength))
	{
		M4W_LOG_ERR("RecvCmd err!");
		return FALSE;
	}
	if (dwMsg != MODULE_MSG_LOGINRET)
	{
		M4W_LOG_ERR("dwMsg err!");
		return FALSE;
	}
	LPLOGIN_RET lpRet = (LPLOGIN_RET)lpData;
	if (lpRet->lRet != ERR_NOERROR)
	{
		M4W_LOG_ERR("lRet err, ret = %d",lpRet->lRet);
		return FALSE;
	}
	M4W_LOG_ERR("lRet len = %d",lpRet->nLength);
	if(lpInfo)
	{
		memcpy(lpInfo,lpRet->lpData,lpRet->nLength);
	}

	PLAYER_INIT_INFO &info = lpInfo->pi;

	//info.nVideoExtSize
	char tag[4] = {0x00, 0x00, 0x00, 0x01};
	fwrite(tag, 1, 4, mwFile);
	fwrite(info.videoExtData+8, 1, 16, mwFile);
	fwrite(tag, 1, 4, mwFile);
	fwrite(info.videoExtData+27, 1, 4, mwFile);
	return TRUE;
}

BOOL CNetDataClient::OpenClient(const char* szIP,int nPort,const char* szPath,int opt, SFILE_INFO* lpFileInfo)
{

	m_szIP = szIP;
	m_nPort = nPort;
	m_szFile = szPath;
	m_opt = opt;
	return ConnectServer(lpFileInfo);
}

void* CNetDataClient::__WorkThread(void* param)
{
	((CNetDataClient*)param)->Work();
	return 0;
}

class CAutoPtr
{
	public:
		CAutoPtr(int len)
		{
			m_pData = (char*)malloc(len);
		}
		~CAutoPtr()
		{
			if(m_pData)
			{
				free(m_pData);
				m_pData = NULL;
			}
		}
		operator char*()
			{
			return m_pData;
			}
		bool operator ==(const char* p)
			{
			  return p == m_pData;
			}
		bool operator !=(const char* p)
			{
			 return p != m_pData;
			}
	private:
		char* m_pData;
};

void CNetDataClient::Work()
{
	M4W_LOG_ERR("CNetDataClient::Work begin.");
	CAutoPtr lpData(1025 * 1024);
	if(!lpData)
	{
		return;
	}

	int nLength = 1025 * 1024;
	DWORD dwMsg;
	DWORD dwIndex;
	time_t tm = time(NULL);

NEXT:
	tm = time(NULL);
	while(1)
	{

//		if(waitforQuit()){
//			M4W_LOG_ERR("quit 1111");
//						break;
//		}
//		if(time(NULL) - tm >= m_nTimeOut * 1000 )
//		{
//			if (OnTimeOut())
//			{
//				M4W_LOG_ERR("quit 222");
//				break;
//			}
//			tm = time(NULL);
//			DefNetMessage(this,MODULE_MSG_PING,0,0,0);
//		}else
		if(CanRead())
		{
//			nLength = 1025 * 1024;
//			if (!RecvCmd(dwMsg,dwIndex,(char*)lpData,nLength))
//			{
//				M4W_LOG_ERR("quit 333");
//				break;
//			}
//			DefNetMessage(this,dwMsg,dwIndex,(char*)lpData,nLength);

			if (!RecvData(dwMsg, dwIndex, (char*)lpData, nLength))
			{
				M4W_LOG_ERR("quit 333");
				break;
			}


			//tm = time(NULL);
		}
	}

	while(!m_evtQuit.Wait(m_cs.getcs(),4000))
	{
		M4W_LOG_ERR("reconnect");
		if(Reconnect())
			goto NEXT;
	}

}

BOOL CNetDataClient::waitforQuit(){
	CSingleLock sLock(&m_cs,TRUE);
	return (m_evtQuit.Wait(m_cs.getcs(),1));
}

BOOL CNetDataClient::Reconnect()
{
	return ConnectServer(NULL);
}

BOOL CNetDataClient::IsPlayerBufferEmpty()
{
	return FALSE;
}

/*void CNetDataClient::SetOrientation(int nOri)
{
	return;
}*/

void CNetDataClient::DefNetMessage(HANDLE hClient,DWORD dwMsg,DWORD dwIndex,char* lpData,int nLength)
{	
	if(dwMsg == MODULE_MSG_EXERET)
	{
		M4W_LOG_ERR("dat=%s,index=%d",lpData,dwIndex);
		CSingleLock slock(&m_cs,TRUE);
		map<long,LPWAIT_INFO>::iterator it = m_mapWait.find(dwIndex);
		if(it != m_mapWait.end())
		{
			memcpy(it->second->szWait,lpData,nLength);
			it->second->nRetLen = nLength;
			it->second->hWait->SetEvent();
		}else{
			M4W_LOG_ERR("not find index");
		}

	}
}

void CNetDataClient::CloseClient()
{
	m_evtQuit.SetEvent();
	CLOSE_THREAD(m_hThread);
	CTcpConnect::Close();
}

BOOL CNetDataClient::Start(DWORD dwStart,DWORD dwEnd)
{
	
	char szCmd[100];
	int len = sprintf(szCmd,"<control name=\"start\" tmstart=\"%d\" tmend=\"%d\" />",dwStart,dwEnd);
	return SendCmd(MODULE_MSG_CONTROL_PLAY,0,szCmd,len + 1);
}

string CNetDataClient::ExecCmd(DWORD dwMsg,const char* szMsg,int nMsgLen)
{
	WAIT_INFO wi;
	memset(&wi,0,sizeof(wi));
	CEvent ew;
	wi.hWait = &ew;
	long nIndex = GetIndex();


	CSingleLock slock(&m_cs,TRUE);
	m_mapWait[nIndex] = &wi;
	slock.Unlock();


	if(! SendCmd(dwMsg,nIndex,(void*)szMsg,nMsgLen))
	{
		map<long,LPWAIT_INFO>::iterator it = m_mapWait.find(nIndex);
		if(it != m_mapWait.end())
		{
			m_mapWait.erase(it);
		}

		return "";
	}

	slock.Lock();
	ew.Wait(m_cs.getcs(), 6000);
	map<long,LPWAIT_INFO>::iterator it = m_mapWait.find(nIndex);
	if(it != m_mapWait.end())
	{
		m_mapWait.erase(it);
	}

	return wi.szWait;
}

void CNetDataClient::StartWork()
{
	if(m_hThread)
		return;

	m_evtQuit.ResetEvent();
	pthread_create(&m_hThread,NULL,__WorkThread,this);
}

/*
void CNetDataClient::DwnlStartWork()
{
	if(m_hDwnlThread)
	{
		MessageBox(NULL,NULL,NULL,MB_OK);
		return;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	m_hDwnlThread = CreateThread(NULL,0,__DwnlWorkThread,this,NULL,NULL);
}
*/

BOOL CNetDataClient::OpenPlayback(const char* szIP,int nPort,const char* szPath,HWND hWnd,CFILE_INFO* lpFileInfo)
{
	return FALSE;
}

/*
BOOL CNetDataClient::Downloadback(const char* szIP,int nPort,const char* szPath,LPPLABACK_FILE_INFO lpFileInfo)
{
	return FALSE;
}*/

DWORD CNetDataClient::GetCurTime()
{
	return 0;
}

BOOL CNetDataClient::Capture(const char* szPath)
{
	return FALSE;
}

void CNetDataClient::SetSpeed(int nSpeed)
{
}

int CNetDataClient::SetPlayPause(BOOL bPause)
{
}

BOOL CNetDataClient::Seek(DWORD dwStart)
{
	return FALSE;
}

BOOL CNetDataClient::OnTimeOut()
{
	return FALSE;
}
 
