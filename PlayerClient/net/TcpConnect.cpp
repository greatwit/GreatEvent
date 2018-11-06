#include "TcpConnect.h"

#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/tcp.h>
#include <unistd.h>

CTcpConnect::CTcpConnect(void)
{
	m_hSocket = INVALID_SOCKET;

}

CTcpConnect::~CTcpConnect(void)
{
	Close();
	/*if (m_hEvent)
	{
		WSACloseEvent(m_hEvent);
		m_hEvent = NULL;
	}*/
}
void CTcpConnect::SetSockOpt(int nLevel,int nName,char* opt,int optlen)
{
	setsockopt(m_hSocket,nLevel,nName,opt,optlen);
}
void CTcpConnect::Close()
{
	if (m_hSocket != INVALID_SOCKET)
	{
	//	WSAEventSelect(m_hSocket,m_hEvent,0);
		close(m_hSocket);
		
		m_hSocket = INVALID_SOCKET;
	}
}
/*HANDLE CTcpConnect::GetEvent()
{
	return m_hEvent;
}*/
BOOL CTcpConnect::Recv(void* lpData,int nLength)
{
	int nRecv = 0;
	int nRet = 0;
	struct timeval tv;
	fd_set fs;
	do 
	{		
		FD_ZERO(&fs);
		FD_SET(m_hSocket,&fs);
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		nRet = select(m_hSocket+1,&fs,NULL,NULL,&tv);
		if (!(nRet > 0 && FD_ISSET(m_hSocket,&fs)))
		{
			M4W_LOG_ERR("recv select err  = %d",nRet);
			return FALSE;
		}
		nRet = recv(m_hSocket,(char*)lpData + nRecv,nLength - nRecv,0);
		if (nRet <= 0)
		{
			M4W_LOG_ERR("recv  err  = %d",nRet);
			/*if (WSAGetLastError() == 10035)
			{
				Sleep(2);
				continue;
			}*/
			return FALSE;
		}
		nRecv += nRet;
	} while (nRecv < nLength);
	return TRUE;
}
BOOL CTcpConnect::Send(void* lpData,int nLength)
{
	int nSend = 0;
	int nRet = 0;
	struct timeval tv;
	fd_set fdWrite;
	do 
	{		
		FD_ZERO(&fdWrite);
		FD_SET(m_hSocket,&fdWrite);
		tv.tv_sec = 6;
		tv.tv_usec = 0;
		nRet = select(m_hSocket+1,NULL,&fdWrite,NULL,&tv);
		if (!(nRet > 0 && FD_ISSET(m_hSocket,&fdWrite)))
		{
			M4W_LOG_ERR("select err: ret = %d",nRet);
			return FALSE;
		}
		nRet = send(m_hSocket,(char*)lpData + nSend,nLength - nSend,0);
		if (nRet <= 0)
		{
			M4W_LOG_ERR("send err nRet = %d",nRet);
			/*if (WSAGetLastError() == 10035)
			{
				Sleep(2);
				continue;
			}*/
			return FALSE;
		}
		nSend += nRet;
	} while (nSend < nLength);
	return TRUE;
}
int CTcpConnect::CanRead()
{
	struct timeval tv;
		fd_set fdRead;
		FD_ZERO(&fdRead);
			FD_SET(m_hSocket,&fdRead);
			tv.tv_sec = 0;
			tv.tv_usec = 100;
			int nRet = select(m_hSocket+1,&fdRead,NULL,NULL,&tv);
			if (!(nRet > 0 && FD_ISSET(m_hSocket,&fdRead)))
			{
				return FALSE;
			}
	return 1;
}
BOOL CTcpConnect::Connect(const char* szIP,int nPort)
{
	Close();
	M4W_LOG_DBG("connect:szIP=%s,port=%d",szIP,nPort);
	m_hSocket = socket(AF_INET,SOCK_STREAM,0);
	if (m_hSocket == INVALID_SOCKET)
	{
		M4W_LOG_ERR("connect: invalid socket!");
		return FALSE;
	}

	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	if( inet_pton(AF_INET,szIP,&addr.sin_addr) <=0 )
	{
		M4W_LOG_ERR("inet_pton error for %s\n",szIP);
		return false;
	}
	//addr.sin_addr.s_addr = inet_addr(szIP);
	int flags = fcntl(m_hSocket, F_GETFL, 0);
	fcntl(m_hSocket, F_SETFL, flags | O_NONBLOCK);

	int nOpt = TRUE;
	setsockopt(m_hSocket,IPPROTO_TCP,TCP_NODELAY,(char*)&nOpt,sizeof(nOpt));

	connect(m_hSocket,(LPSOCKADDR)&addr,sizeof(sockaddr_in));
	fd_set fdWrite;
	FD_ZERO(&fdWrite);
	FD_SET(m_hSocket,&fdWrite);

	struct timeval timeout;
	timeout.tv_sec = 4;
	timeout.tv_usec = 0;
	int nRet = select(m_hSocket+1,NULL,&fdWrite,NULL,&timeout);
	if (!(nRet > 0 && FD_ISSET(m_hSocket,&fdWrite)))
	{		
		M4W_LOG_ERR("connect:ret=%d",nRet);
		return FALSE;
	}
	return TRUE;
}
