
#ifndef I_RECV_BASE_H_
#define I_RECV_BASE_H_

#include <stdlib.h>

class ISendBase
{
	public:
		virtual ~ISendBase(){}

		virtual int	create(short localPort) = 0;
		virtual int	release() = 0;
		virtual int connect(char* sDestIp, short destPort) = 0;
		virtual int disConnect() = 0;
		virtual int sendBuffer(void*buff, int dataLen, int64_t timeStamp) = 0;
};

#endif
