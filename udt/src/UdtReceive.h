/*
 * Create on oct 19, 2018
 */

#ifndef UdtReceive_H_
#define UdtReceive_H_

#include "IRecvBase.h"

class UdtReceive:public IRecvBase
{
	public:
		UdtReceive();
		virtual ~UdtReceive();
		virtual int	create(short localPort);
		virtual int	release();
		virtual void	registerCallback(IReceiveCallback *base);
		virtual int	startThread();
		virtual int	stopThread();

	private:
//		int createServer(UDTSOCKET &serv, char* port);
//		int releaseServer();
//		int recvEx(UDTSOCKET usock, char*buf, int size);

	private:
		//UDTSOCKET mSockHandle;
		bool mRunning;
};

#endif
