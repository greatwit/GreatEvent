

#include "TcpClient.hpp"
#include "common.h"
#include "Ioutils.hpp"

TcpClient :: TcpClient( )
		:mSockId(0)
{

}

TcpClient :: ~TcpClient()
{

}

int TcpClient :: connect(const char* destIp, unsigned short destPort) {
	int ret = IOUtils::tcpConnect(destIp, destPort, &mSockId, 0);
	GLOGD("connect ret:%d sockid:%d.",ret, mSockId);
	return ret;
}

int TcpClient :: disConnect() {
	return 0;
}

