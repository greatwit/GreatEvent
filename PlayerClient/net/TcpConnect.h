#pragma once
#include "../stdafx.h"
#define INVALID_SOCKET -1
class CTcpConnect
{
public:
	CTcpConnect(void);
public:
	~CTcpConnect(void);
	BOOL Connect(const char* szIP,int nPort);
	void Close();
	int CanRead();
	BOOL Send(void* lpData,int nLength);
	BOOL Recv(void* lpData,int nLength);
	//HANDLE GetEvent();
	void SetSockOpt(int nLevel,int nName,char* opt,int optlen);
	//WSAEVENT m_hEvent;
private:
	int m_hSocket;
	//WSAEVENT m_hEvent;
};
