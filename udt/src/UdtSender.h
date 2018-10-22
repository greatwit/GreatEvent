/*
 * Create on oct 19, 2018
 */

#ifndef UdtSender_H_
#define UdtSender_H_

#include "ISendBase.h"

class UdtSender : public ISendBase
{
	public:
	UdtSender();
		virtual ~UdtSender();
		virtual int	create(short localPort);
		virtual int	release();
		virtual int connect(char* sDestIp, short destPort);
		virtual int disConnect();
		virtual int sendBuffer(void*buff, int dataLen, int64_t timeStamp);


		int getSockHandle();

	public:
		static void* sendfileProc(void* sender);
		//bool mRunning;
};

#endif
