#ifndef __tcpclient_hpp__
#define __tcpclient_hpp__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



// half-sync/half-async thread pool server
class TcpClient {
public:
	TcpClient( );
	virtual ~TcpClient();

	int connect(const char* destIp, unsigned short destPort);
	int disConnect();

private:
	int 			mSockId;
};


#endif
