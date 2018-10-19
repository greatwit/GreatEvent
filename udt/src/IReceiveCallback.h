
#ifndef I_RECEIVE_CALLBACK
#define I_RECEIVE_CALLBACK

#include <stdlib.h>

class IReceiveCallback
{
	public:
		virtual ~IReceiveCallback(){}
		virtual void ReceiveSource(int64_t timeStamp, char*mimeType, void* buffer, int dataLen)=0;
};

#endif
