// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

typedef const char* LPCTSTR;

/*
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

#include<time.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
*/

#include <string>
#include <map>
#include <sys/time.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "M4wLog.h"

#define HWND void*
#define BOOL int
#define TRUE 1
#define FALSE 0
#define HANDLE void*
#define UINT unsigned int
#define BYTE unsigned char
#define PBYTE unsigned char*
#define SOCKET int
#define INVALID_HANDLE_VALUE -1
#define LPVOID void*
#define MAX_PATH 256
#define ZeroMemory(p,t) memset(p,0,t)
typedef unsigned int DWORD;
#define WINAPI
#define CLOSE_THREAD(th) \
{\
	if(th)\
	{\
	pthread_join(th,NULL);\
	}\
}
typedef struct sockaddr* LPSOCKADDR;
#define NET_FLAG 0xfefdfcfb



#define FRAME_VIDEO_I	1
#define FRAME_VIDEO_P	2
#define FRAME_AUDIO		3
#define FRAME_TALKAUDIO			4

#define AUDCODEC_G711_ULAW 1
#define AUDCODEC_G711_ALAW 2
#define AUDCODEC_PCM 3
#define AUDCODEC_AAC 4



#define strlwr(s)\
{\
	char *str; \
	str = s; \
	while(*str != '\0') \
	 {  \
	  if(*str >= 'A' && *str <= 'Z') { \
	     *str += 'a'-'A'; \
	 }  \
	 str++; \
	 }\
}

typedef struct tagAV_FRAME
{
	DWORD dwFrameType;
	int   nLength;
	DWORD dwTick;
	DWORD dwTm;
	char  lpData[];
}AV_FRAME,*LPAV_FRAME;

#define CALLBACK
typedef void (CALLBACK* DEFNETMESSAGE)(HANDLE hClient,DWORD dwMsg,DWORD dwIndex,char* lpData,int nLength,void* param);
typedef void (CALLBACK *ONDOWNLAODSTATUS)(HANDLE hDownload,const char* szSrcFile,const char* szDstFile,DWORD dwPos,DWORD dwTotal,void* param);
typedef void (CALLBACK* ONRECVAVDATA)(void* param,LPAV_FRAME lpFrame,BOOL dataEnd,BOOL bSend);
typedef void (CALLBACK* ONRECVFILEDATA)(void* param,void* lpFrame,int len,BOOL dataEnd);
typedef int (CALLBACK* GetEmptyBufferSize)(void* param);


/*class CRect : public RECT
{
public:
	CRect()
	{
		left = 0;
		bottom = 0;
		right = 0;
		top = 0;
	}
	CRect(RECT& rc)
	{
		left = rc.left;
		bottom = rc.bottom;
		right = rc.right;
		top = rc.top;
	}
	void SetRect(int l,int t,int r,int b)
	{
		left = l;
		bottom = b;
		right = r;
		top = t;
	}
	int Width()
	{
		return right - left;
	}
	int Height()
	{
		return bottom - top;
	}
};*/

class CCriticalSection
{
public:
	CCriticalSection()
	{
		pthread_mutex_init(&m_cs,NULL);
	}
	~CCriticalSection()
	{
		pthread_mutex_destroy(&m_cs);
	}
	void Lock()
	{
		pthread_mutex_lock(&m_cs);
	}
	void Unlock()
	{
		pthread_mutex_unlock(&m_cs);
	}
	pthread_mutex_t* getcs(){return &m_cs;}
private:
	pthread_mutex_t m_cs;
};
class CSingleLock
{
public:
	CSingleLock(CCriticalSection* pCS,BOOL bLock)
	{
		m_bLock = bLock;
		m_pCS = pCS;
		if(bLock)
		{
			m_pCS->Lock();
		}
	}
	~CSingleLock()
	{
		if(m_bLock)
		{
			m_pCS->Unlock();
		}
	}

	void Lock()
	{
		if(m_bLock)
			return;
		m_pCS->Lock();
		m_bLock = TRUE;
	}

	void Unlock()
	{
		if(!m_bLock)
			return;
		m_pCS->Unlock();
		m_bLock = FALSE;
	}
private:
	CCriticalSection* m_pCS;
	BOOL m_bLock;
};


class CEvent 
{
	

// Constructor
public:
	/* explicit */ CEvent()
	{
		pthread_cond_init(&m_hEvent,NULL);
		m_bSignal = FALSE;
	}

	BOOL Wait(pthread_mutex_t* mtx,int tmWait)
	{
		if(m_bSignal)
		{
			return TRUE;
		}

		struct timeval now;
		timespec ts;
		 gettimeofday(&now, NULL);

		ts.tv_nsec = now.tv_usec * 1000+(tmWait % 1000) * 1000000;
		ts.tv_sec = now.tv_sec +tmWait / 1000;
		return pthread_cond_timedwait(&m_hEvent,mtx,&ts) == 0;
	}

// Operations
public:
	BOOL SetEvent()
	{
		if(!m_bSignal)
		{
			pthread_cond_signal(&m_hEvent);
			m_bSignal = TRUE;
		}
		return m_bSignal;
	}
	
	BOOL ResetEvent()
	{
		m_bSignal = FALSE;
		return FALSE;
	}

	operator HANDLE()
	{
		return &m_hEvent;
	}
// Implementation
public:
	virtual ~CEvent()
	{
		pthread_cond_destroy(&m_hEvent);
	}
private:
	pthread_cond_t m_hEvent;
	BOOL m_bSignal;
};




#include<list>
#include<map>
using namespace std;




typedef struct tagPLAYER_INIT_INFO
{
	int nCodeID;
	int nFps;
	int nWidth;
	int nHeigth;
	int nSampleRate;
	int nBistspersample;
	int nChannel;
	int nAudioFormat;
	int sample_fmt;
	unsigned long long channel_layout;//8 byte
	int nCodecFlag;
	int bits_per_sample;
	int bit_rate;
	int me_method;
	int bit_ratetolerance;
	int block_align;

	int gop_size;
	int frame_size;
	int frame_number;
	int ildct_cmp;
	int me_subpel_quality;
	int mb_lmax;
	int mb_lmin;
	int me_penalty_compensation;
	float qblur;
	int  flags;
	int extsize;
	char extdata[100];
	int  nVideoExtSize;
	char	videoExtData[100];
}PLAYER_INIT_INFO,*LPPLAYER_INIT_INFO;



typedef struct tagCFILE_INFO
{
	PLAYER_INIT_INFO pi;
	DWORD tmStart;
	DWORD tmEnd;
	ONDOWNLAODSTATUS proc;
	ONRECVAVDATA onav;
	ONRECVFILEDATA onfile;
	GetEmptyBufferSize getbufSize;
	void* param;
}CFILE_INFO,*LPCFILE_INFO;

typedef struct tagSFILE_INFO
{
	PLAYER_INIT_INFO pi;
	DWORD tmStart;
	DWORD tmEnd;
}SFILE_INFO,*LPSFILE_INFO;
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�




enum MODULE_MSG_ID{
	MODULE_MSG_PING,
	MODULE_MSG_LOGIN ,
	MODULE_MSG_LOGINRET,
	MODULE_MSG_ADDMDL,
	MODULE_MSG_DELMDL,
	MODULE_MSG_GETDEV,
	MODULE_MSG_DEVINFO,
	MODULE_MSG_GETSERVER,
	MODULE_MSG_SERINFO,
	MODULE_MSG_GETSSERVER,
	MODULE_MSG_SSERINFO,
	MODULE_MSG_GETSERVICE,
	MODULE_MSG_SERVICEINFO,
	MODULE_MSG_ADDSERVER,
	MODULE_MSG_MODSERVER,
	MODULE_MSG_DELSERVER,
	MODULE_MSG_ADDSSVR,
	MODULE_MSG_MODSSVR,
	MODULE_MSG_DELSSVR,
	MODULE_MSG_ADDSVC,
	MODULE_MSG_DELSVC,
	MODULE_MSG_ADDDEV,
	MODULE_MSG_MODDEV,
	MODULE_MSG_DELDEV,
	MODULE_MSG_POINTVALUE,//������ݡ�����������״̬��������Ϣ�ȡ���Ŀǰ����������������״̬�������벻����
	MODULE_MSG_VIDEO,
	MODULE_MSG_CHCOUNT,
	MODULE_MSG_GETINDEX,
	MODULE_MSG_EXERET,
	MODULE_MSG_STARTSERVICE,
	MODULE_MSG_STOPSVC,
	MODULE_MSG_WRITEPV,
	MODULE_MSG_MANAGEUSERLOGIN,
	MODULE_MSG_GETSVRUSER,
	MODULE_MSG_USERRET,
	MODULE_MSG_AddUser,//����û�
	MODULE_MSG_ModifyUser,//�޸��û�
	MODULE_MSG_DeleteUser,//ɾ���û�
	MODULE_MSG_QueryUser,//��ѯ�û�
	MODULE_MSG_AddUserAuthority,//����û�Ȩ��
	MODULE_MSG_ModifyUserAuthority,//�޸��û�Ȩ��
	MODULE_MSG_DeleteUserAuthority,//ɾ���û�Ȩ��
	MODULE_MSG_QueryUserAuthority,//��ѯ�û�Ȩ��
	MODULE_MSG_QueryLoginfo,//��ѯ��־--------------����Ϊ���������
	MODULE_MSG_QueryLoginfoResponse,//��ѯ��־��Ӧ-----------------����Ϊ����������
	MODULE_MSG_QueryUserResponse,//��ѯ�û���Ӧ
	MODULE_MSG_QueryUserAuthorityResponse,//��ѯ�û�Ȩ����Ӧ
	MODULE_MSG_SYSUSERLOGIN,//���ĵ�¼���������
	MODULE_MSG_AddLoginfo,//�����־
	MODULE_MSG_AddModuleType,//��ӷ���ģ������
	MODULE_MSG_DeleteModuleType,//ɾ������ģ������
	MODULE_MSG_AddService_Device,//��ӷ������豸�İ�
	MODULE_MSG_DeleteService_Device,//ɾ���������豸�İ�
	MODULE_MSG_GETAUDIOHEAD,
	MODULE_MSG_AUDIODATA,
	MODULE_MSG_ModifyService_Device,//�޸ķ������豸�İ�
	MODULE_MSG_MODDEVRESPONSE,//�޸ķ������豸��Ӧ
	MODULE_MSG_STARTSEND,
	MODULE_MSG_ENDSEND,
	MODULE_MSG_DEVPOINT,
	MODULE_MSG_SETPARAM,//�����豸����
	MODULE_MSG_GETPARAM,//��ȡ�豸����
	MODULE_MSG_DEVPARAM,
	MODULE_MSG_MANAGERGETSERVER,//����ͻ��˻�ȡҵ�����˷����豸��Ϣ
	MODULE_MSG_MANAGERSERINFO,//����ͻ��˻�ȡҵ�����˷����豸��Ϣ
	MODULE_MSG_MANAGERINFO,//����ͻ��˻�ȡҵ�����˷����豸��Ϣ
	MODULE_MSG_GETDEVSTATUS,
	MODULE_MSG_DEVSTATUS,
	MODULE_MSG_CONTROL_PLAY,
	MODULE_MSG_DATAEND,
	MODULE_MSG_SECTION_END,
	MODULE_MSG_GETDEVICE_SVR_INFO,//ͨ��sn��ȡ���Ѱ󶨵�������豸������Ӧ��ҵ�������ip�����·����Ӧ��sip����Ķ˿�22103
	MODULE_MSG_RESDEVICE_SVR_INFO,
	MODULE_MSG_DEVWORKST,
	MODULE_MSG_OPENAUIDO,
	MODULE_MSG_CLOSEAUDIO,
	MODULE_MSG_USERMSGDATA,
	MODULE_MSG_UMDRET,
	MODULE_MSG_SENDMESSAGE, // ��Ϣ���͵��豸��
	MODULE_MSG_GETOPENVIDEOPARAM,//��ȡ����Ƶ��صĲ���
	MODULE_MSG_ONSYSTEM_LONGIN,//��ʼ������Ϣ�����ip�Ͷ˿ڵ���Ϣ��
	MODULE_MSG_RESPONSEMESSAGE_IP_PORT_INFO,//��ȡ��Ϣ�����ip�Ͷ˿ڵ���Ϣ��
	MODULE_MSG_REQUESTMESSAGE_IP_PORT_INFO,//�ظ���Ϣ�����ip�Ͷ˿ڵ���Ϣ��
	MODULE_MSG_DEV_REGISTER_MESSAGE,//�豸ע�ᵽ��Ϣ����                              ms��Ҫ���͵���Ϣ
	MODULE_MSG_DEV_UNREGISTER_MESSAGE,//�豸ע������Ϣ����                            ms��Ҫ���͵���Ϣ
	MODULE_MSG_GETMSGSVR,//׼��ͬ���б�                                              ms��Ҫ���͵���Ϣ
	MODULE_MSG_GETDEVLIST,//ͬ���б�׼��ok�����������Է���Ϣ�������ҿ���ת����
	MODULE_MSG_TRANMSG,
	MODULE_MSG_USERMSG,
	MODULE_MSG_GETMSGSERVICEVALUE,//��Ϣ����������豸����״��
	MODULE_MSG_SEEK_CMPD,

};
#define ERR_NOERROR		0
#define ERR_FUNADDR		1
#define ERR_NEWMEM		2
#define ERR_PARXML		3
#define ERR_NONEEDXML	4
#define ERR_PARAM		5
#define ERR_NOSURPORT	6
#define ERR_FAILE		7
#define ERR_DISCONNECT	8
#define ERR_MAXCOUNT	9 //�þ�Ա�Ѿ��ͱ����ڶԽ��У���ʱ�޷�֧�ֶԽ�
#define ERR_INITDB		10
#define ERR_SNDERROR	11
#define ERR_NETSERVER	12
#define ERR_CONNECT		13
#define ERR_NETSEND		14
#define ERR_QUERYDB		15
#define ERR_OBJERROR	16
#define ERR_NETRECV		17
#define ERR_MSGERROR	18
#define ERR_NOOBJECT	19 //���豸��֧�ֶԽ�
#define ERR_NOTWORK		20
#define ERR_DEVTYPE		21
#define ERR_OBJEXIST	22
#define ERR_TIMEOUT		23
typedef struct tagFILE_DATA
{
	DWORD dwPos;
	int nLength;
	char lpData[];
}FILE_DATA,*LPFILE_DATA;


typedef struct tagMediaInfo
{
	int client;
	int param;
	int   fps;
	int w;
	int h;
	int sr;
	int ch;
	int bitrate;
	int bit;
	int afmt;
	int start;
	int end;
	int aExtLen;
	char  aExtData[20];
	int vExtLen;
	char vExtData[100];
}MINFO,*LPMINFO;

typedef struct tagNET_CMD
{
	DWORD dwFlag;
	DWORD dwCmd;
	DWORD dwIndex;
	DWORD dwLength;
	char  lpData[];
}NET_CMD,*LPNET_CMD;

typedef struct tagLOGIN_RET
{
	DWORD lRet;
	int nLength;
	char lpData[1024];
}LOGIN_RET,*LPLOGIN_RET;

