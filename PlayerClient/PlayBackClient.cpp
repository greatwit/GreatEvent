#include "stdafx.h"
#include "PlayBackClient.h"

#define MAX_EMP_SIZE  520
#define MIN_EMP_SIZE  200

CPlayBackClient::CPlayBackClient(void)// : m_evtSeek()
{
	
//	m_hPlayer = NULL;
	m_avBuf.ExtendBuffer(10 * 1024);
	m_bDataEnd = FALSE;
	m_dwCurPts = 0;
	m_bSend    = FALSE;
	m_bPause   = FALSE;
	param 	   = NULL;
	proc	   = NULL;
	getEmpBufSize = NULL;

//	cnt = 0;

}


CPlayBackClient::~CPlayBackClient(void)
{
	CloseClient();
}

/*
void CPlayBackClient::SetFramePlay(BOOL bFramePlay)
{
	if(!m_hPlayer)
		return;
	PLAYER_SetFramePlay(m_hPlayer,bFramePlay);
}
void CPlayBackClient::SetVol(DWORD dwVol)
{
	if(!m_hPlayer)
		return;
	PLAYER_SetVolume(m_hPlayer,dwVol);
}
void CPlayBackClient::SetSpeed(int nSpeed)
{
	if(!m_hPlayer)
		return;
	PLAYER_SetSpeed(m_hPlayer,nSpeed);
}
*/


void CPlayBackClient::sendControl(){
	if(true)
		return;
	int empSize = getEmpBufSize(param);
			if(empSize<0){
				M4W_LOG_ERR("empSize err!%d",empSize);
			}
			if(/*m_bufData.GetDataCount() == 0 &&*/empSize>MIN_EMP_SIZE)
			{

				/*PLAYER_InputAVData(this,lpFrame->lpData,lpFrame->nLength,lpFrame->dwFrameType,
					lpFrame->dwTick,lpFrame->dwTm);*/
				if(empSize>MAX_EMP_SIZE) {
					/*M4W_LOG_DBG("pause--->%d,m_bDataEnd-->%d",m_bPause,m_bDataEnd);
					if(!m_bDataEnd&&m_bPause){

						char szCmd[100];
						int len = sprintf(szCmd,"<control name=\"setpause\" value=\"0\"/>");
						m_bPause = !SendCmd(MODULE_MSG_CONTROL_PLAY,0,szCmd,len);
						M4W_LOG_ERR("pause--->false,sendCmd-->%d",!m_bPause);
					}*/
				}


			}else{
				if(!m_bDataEnd /*&& !m_bSend*/&&!m_bPause){
					char szCmd[100];
					int len = sprintf(szCmd,"<control name=\"setpause\" value=\"1\"/>");
					int ret = SendCmd(MODULE_MSG_CONTROL_PLAY,0,szCmd,len);
					m_bPause = ret;
					m_bSend = ret;
					M4W_LOG_ERR("pause--->true,sendCmd-->%d,m_bPause-->%d",m_bSend,m_bPause);
				}
			}
}

void CPlayBackClient::DefNetMessage(HANDLE hClient,DWORD dwMsg,DWORD dwIndex,char* lpData,int nLength)
{
	CNetDataClient::DefNetMessage(hClient,dwMsg,dwIndex,lpData,nLength);
	M4W_LOG_ERR("MSG:%d",dwMsg);
	if(dwMsg == MODULE_MSG_SEEK_CMPD){
			M4W_LOG_ERR("MODULE_MSG_SEEK_CMPD");
			//TODO clear player buffer
		return;
	}
	if(dwMsg == MODULE_MSG_DATAEND)
	{
		M4W_LOG_ERR("data end!");
		m_bDataEnd = TRUE;
		if(proc){
			proc(param,NULL,TRUE,m_bSend);
		}
		return;
	/*	if (m_hPlayer)
		{
			PLAYER_SetDataEnd(m_hPlayer);
		}*/
	}else if(dwMsg == MODULE_MSG_SECTION_END)//10m end
	{
		M4W_LOG_ERR("10m end!");
		CSingleLock slock(&m_csbSend,TRUE);
		m_bSend = FALSE;

		m_bPause  = true;//todo

		if(proc){
			proc(param,NULL,FALSE,m_bSend);
		}

		/*int empSize = getEmpBufSize((int)param);
		if(empSize<0){
			M4W_LOG_ERR("empSize err!%d",empSize);
			return;
		}
		if(empSize>MIN_EMP_SIZE)
		{
			if(!m_bDataEnd){
				char szCmd[100];
				int len = sprintf(szCmd,"<control name=\"setpause\" value=\"0\"/>");
				int ret = SendCmd(MODULE_MSG_CONTROL_PLAY,0,szCmd,len);
				m_bPause = !ret;
				m_bSend = ret;
				M4W_LOG_ERR("sendCmd-->%d,m_bPause-->%d",m_bSend,m_bPause);
			}

		}*/
		return;

	}else if(dwMsg == MODULE_MSG_PING){

		return;
	}
	//CSingleLock slock(&m_cs,TRUE);
	//if(m_evtSeek.Wait(m_cs.getcs(),0))
	//	return;

	if(dwMsg == MODULE_MSG_VIDEO)
	{
		LPAV_FRAME lpFrame = (LPAV_FRAME)lpData;
		m_dwCurPts = lpFrame->dwTick;
	//	if(m_hPlayer)

			/*cnt++;
			if(cnt>=400){
				CSingleLock slock(&m_csbSend,TRUE);
				m_bSend = FALSE;
				cnt = 0;
			}*/

			if(!getEmpBufSize)
				return;

			if(proc){
				proc(param,lpFrame,FALSE,TRUE/*m_bSend*/);
			}
			sendControl();

		return ;

		m_bufData.WriteData(lpData,nLength);		
	}

	M4W_LOG_ERR("msg:%d",dwMsg);
	return;
	//if(m_hPlayer)
	{
		LPAV_FRAME lpFrame = NULL;
		while(m_bufData.GetDataCount() > 0)
		{
			M4W_LOG_ERR("ERR!");
			/*if(m_evtSeek.Wait(m_cs.getcs(),0))
				return;*/
			lpFrame = (LPAV_FRAME)m_bufData.GetHead();
			if (!lpFrame)
			{
				break;
			}
			/*if (PLAYER_GetEmptyBufferSize(m_hPlayer,!(lpFrame->dwFrameType == FRAME_AUDIO)) < lpFrame->nLength)
			{
				break;
			}					
			PLAYER_InputAVData(m_hPlayer,lpFrame->lpData,lpFrame->nLength,lpFrame->dwFrameType,
				lpFrame->dwTick,lpFrame->dwTm);*/

			m_bufData.RemoveHead();
		}
	}
	if(m_bufData.GetDataCount() < 400 && !m_bDataEnd && !m_bSend)
	{		
		M4W_LOG_ERR("ERR 111111");
			char szCmd[100];
			int len = sprintf(szCmd,"<control name=\"setpause\" value=\"0\"/>");
			m_bSend = SendCmd(MODULE_MSG_CONTROL_PLAY,0,szCmd,len);		
	}
}

int CPlayBackClient::OpenPlayback(const char* szIP,int nPort,const char* szPath,HWND hWnd, CFILE_INFO* cfi)
{
	SFILE_INFO fi;
	memset(&fi,0,sizeof(SFILE_INFO));
	if(!OpenClient(szIP,nPort,szPath,0,&fi))
	{
		M4W_LOG_ERR("OpenPlayback err:szIP=%s,port=%d",szIP,nPort);
		return FALSE;
	}
	proc = cfi->onav;
	getEmpBufSize = cfi->getbufSize;
	param = cfi->param;
//	memcpy((void*)(&cfi->pi),(void*)(&fi.pi),sizeof(fi.pi));
//	memcpy((void*)(cfi->pi.extdata),(void*)(fi.pi.extdata),20);
//	memcpy((void*)(cfi->pi.videoExtData),(void*)(fi.pi.videoExtData),100);
	cfi->pi = fi.pi;
	cfi->tmStart = fi.tmStart;
	cfi->tmEnd = fi.tmEnd;

//	M4W_LOG_ERR("start = %d,end = %d,bitrate=%d--->bitsper=%d---->bit=%d,frame_size = %d,chan_layout=%lld,extsize = %d,videoextsize = %d,"
//			"w = %d,h = %d,samprate=%d,afmt=%d",
//			fi.tmStart,fi.tmEnd,fi.pi.bit_rate,
//			fi.pi.bits_per_sample,fi.pi.nBistspersample,
//			fi.pi.frame_size,fi.pi.channel_layout,fi.pi.extsize,fi.pi.nVideoExtSize,fi.pi.nWidth,fi.pi.nHeigth,fi.pi.nSampleRate,fi.pi.nAudioFormat);

	/*m_hPlayer = PLAYER_CreatePlayer(hWnd,&fi.pi);
	if(!m_hPlayer)
	{
		return FALSE;
	}*/

	/*PLAYER_SetPlayMode(m_hPlayer,TRUE);
	PLAYER_SetVolume(m_hPlayer,0xffffffff);//0xffff03e8);*/

	m_dwCurPts = 0;
	StartWork();
	return TRUE;
}
BOOL CPlayBackClient::IsPlayerBufferEmpty()
{
	if (!m_bDataEnd)
	{
		return FALSE;
	}
	/*if (!m_hPlayer)
	{
		return TRUE;
	}*/
	//return PLAYER_IsDataEmpty(m_hPlayer);
	return true;//todo
}

void CPlayBackClient::CloseClient()
{
	CNetDataClient::CloseClient();
	/*if(m_hPlayer)
	{
		PLAYER_ReleasePlayer(m_hPlayer);
		m_hPlayer = NULL;
	}*/
}

BOOL CPlayBackClient::Reconnect()
{
	if(!CNetDataClient::Reconnect())
		return FALSE;

//	m_bufData.ClearBuffer();
//	PLAYER_ClearBuffer(m_hPlayer);	
	m_bDataEnd = FALSE;
	m_bSend = FALSE;
	m_bPause = FALSE;
	//cnt = 0;
	return Start(m_dwCurPts,0);
}

/*DWORD CPlayBackClient::GetCurTime()
{
	if(!m_hPlayer)
		return 0;
	return PLAYER_GetCutTime(m_hPlayer);
}*/

BOOL CPlayBackClient::Start(DWORD dwStart,DWORD dwEnd)
{
	m_dwCurPts = dwStart;
	return CNetDataClient::Start(dwStart,dwEnd);
}

int CPlayBackClient::SetPlayPause(BOOL bPause)
{

	CSingleLock slock(&m_csbSend,TRUE);
	if(m_bPause == bPause)
			return TRUE;
	if(!m_bDataEnd /*&& !m_bSend*/)
	{

		char szCmd[100];
		int len;
		if(!bPause)
		    len = sprintf(szCmd,"<control name=\"setpause\" value=\"0\"/>");
		else{
			if(m_bSend)
				return FALSE;

			len = sprintf(szCmd,"<control name=\"setpause\" value=\"1\"/>");
		}
		int ret = SendCmd(MODULE_MSG_CONTROL_PLAY,0,szCmd,len);
		m_bSend = ret;
		if(ret)
			m_bPause = bPause;
		M4W_LOG_ERR("setPlayPause:%d,ret--->%d",bPause,ret);
		return m_bSend;
	}
	return FALSE;

}

BOOL CPlayBackClient::Seek(DWORD dwStart)
{
	//m_bDataEnd = TRUE;
	char szCmd[256];
	
	//m_evtSeek.SetEvent();
	int len = sprintf(szCmd,"<control name=\"seek\" value=\"%d\"/>",dwStart);
	M4W_LOG_ERR("seek to %d",dwStart);
	string s =ExecCmd(MODULE_MSG_CONTROL_PLAY,szCmd,len + 1);

	if(s.empty())
	{
		return FALSE;
	}

	//PLAYER_SetPause(m_hPlayer,TRUE);
//	m_bufData.ClearBuffer();
//	PLAYER_ClearBuffer(m_hPlayer);
	//PLAYER_SetPause(m_hPlayer,FALSE);
	m_bDataEnd = FALSE;
	m_bSend = FALSE;

	//m_evtSeek.ResetEvent();
	
	return TRUE;

	//m_bDataEnd = FALSE; 
	//len = sprintf(szCmd,"<control name=\"setpause\" value=\"0\"/>");	
//	YTNET_Send(m_hClient,MODULE_MSG_CONTROL_PLAY,0,szCmd,len + 1 );
}

/*
BOOL CPlayBackClient::Capture(const char* szPath)
{
	if(!m_hPlayer)
		return FALSE;
	return PLAYER_Capture(m_hPlayer,szPath);
}

void CPlayBackClient::SetOrientation(int nOri)
{
	if(!m_hPlayer)
		return;
	PLAYER_SetOrientation(m_hPlayer,nOri);
}*/
