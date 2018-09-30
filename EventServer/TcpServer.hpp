


#ifndef __tcpserver_hpp__
#define __tcpserver_hpp__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


struct event;

// half-sync/half-async thread pool server
class TcpServer {
public:
	TcpServer( const char * bindIP, int port );
	~TcpServer();

	void setTimeout( int timeout );
	void setMaxConnections( int maxConnections );
	void setMaxThreads( int maxThreads );
	void setReqQueueSize( int reqQueueSize, const char * refusedMsg );

	void shutdown();
	int isRunning();
	int run();
	void runForever();

private:

	char mBindIP[ 64 ];
	int mPort;
	int mIsShutdown;
	int mIsRunning;

	int mTimeout;
	int mMaxThreads;
	int mMaxConnections;
	int mReqQueueSize;
	char * mRefusedMsg;

	static void * eventLoop( void * arg );

	int start();

	static void sigHandler( int, short, void * arg );

	static void outputCompleted( void * arg );
};

#endif

