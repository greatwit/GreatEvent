
#ifndef I_RECV_BASE_H_
#define I_RECV_BASE_H_

#include "IReceiveCallback.h"

class IRecvBase
{
	public:
		virtual ~IRecvBase(){}
		virtual int	create(short localPort) = 0;
		virtual int	release() = 0;
		virtual void	registerCallback(IReceiveCallback *base) = 0;
		virtual int	startThread() = 0;
		virtual int	stopThread() = 0;
};

#endif
