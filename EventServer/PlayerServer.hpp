

#ifndef __player_server_hpp__
#define __player_server_hpp__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "EventCall.hpp"
#include "TaskBase.hpp"

#include "event.h"

// half-sync/half-async thread pool server
class PlayerServer {
public:
	PlayerServer( const char * bindIP, int port );
	~PlayerServer();

	int registerEvent(const EventArg& evarg);
	void shutdown();

	void setMaxConnections( int maxConnections );
	void setMaxThreads( int maxThreads );
	void setReqQueueSize( int reqQueueSize, const char * refusedMsg );

/*	const EventArg& getEventArg();

	void shutdown();
	int isRunning();
	int run();
	void runForever();*/

private:

	char mBindIP[ 64 ];
	int mPort;
	int mIsShutdown;
	int mIsRunning;
	int mListenFD;


	int mMaxThreads;
	int mMaxConnections;
	int mReqQueueSize;
	char * mRefusedMsg;

	AcceptArg_t mAcceptArg;
	struct event mEvAccept;

//	static void * eventLoop( void * arg );
//	int start();
//	static void sigHandler( int, short, void * arg );
//	static void outputCompleted( void * arg );
};

#endif

