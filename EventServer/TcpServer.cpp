

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/tcp.h>

#include "TcpServer.hpp"

//#include "sphandler.hpp"
//#include "spexecutor.hpp"

#include "EventCall.hpp"
#include "Session.hpp"

#include "sputils.hpp"
#include "Ioutils.hpp"

#include "config.h"

#include "event.h"
#include "common.h"

//#include "event_msgqueue.h"

TcpServer :: TcpServer( const char * bindIP, int port )
{
	snprintf( mBindIP, sizeof( mBindIP ), "%s", bindIP );
	mPort = port;
	mIsShutdown = 0;
	mIsRunning = 0;

	mTimeout = 600;
	mMaxThreads = 64;
	mReqQueueSize = 128;
	mMaxConnections = 256;
	mRefusedMsg = strdup( "System busy, try again later." );
}

TcpServer :: ~TcpServer()
{
	if( NULL != mRefusedMsg ) free( mRefusedMsg );
	mRefusedMsg = NULL;
}

void TcpServer :: setTimeout( int timeout )
{
	mTimeout = timeout > 0 ? timeout : mTimeout;
}

void TcpServer :: setMaxThreads( int maxThreads )
{
	mMaxThreads = maxThreads > 0 ? maxThreads : mMaxThreads;
}

void TcpServer :: setMaxConnections( int maxConnections )
{
	mMaxConnections = maxConnections > 0 ? maxConnections : mMaxConnections;
}

void TcpServer :: setReqQueueSize( int reqQueueSize, const char * refusedMsg )
{
	mReqQueueSize = reqQueueSize > 0 ? reqQueueSize : mReqQueueSize;

	if( NULL != mRefusedMsg ) free( mRefusedMsg );
	mRefusedMsg = strdup( refusedMsg );
}

void TcpServer :: shutdown()
{
	mIsShutdown = 1;
}

int TcpServer :: isRunning()
{
	return mIsRunning;
}

int TcpServer :: run()
{
	int ret = -1;

	pthread_attr_t attr;
	pthread_attr_init( &attr );
	assert( pthread_attr_setstacksize( &attr, 1024 * 1024 ) == 0 );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

	pthread_t thread = 0;
	ret = pthread_create( &thread, &attr, reinterpret_cast<void*(*)(void*)>(eventLoop), this );
	pthread_attr_destroy( &attr );
	if( 0 == ret ) {
		GLOGE( "Thread #%ld has been created to listen on port [%d]", thread, mPort );
	} else {
		mIsRunning = 0;
		GLOGE( "Unable to create a thread for TCP server on port [%d], %s",
			mPort, strerror( errno ) ) ;
	}

	return ret;
}

void TcpServer :: runForever()
{
	//eventLoop( this );
	run();
}

void * TcpServer :: eventLoop( void * arg )
{
	TcpServer * server = (TcpServer*)arg;

	server->mIsRunning = 1;

	server->start();

	server->mIsRunning = 0;

	return NULL;
}

void TcpServer :: sigHandler( int, short, void * arg )
{
	TcpServer * server = (TcpServer*)arg;
	server->shutdown();
}

void TcpServer :: outputCompleted( void * arg )
{
/*
	SP_CompletionHandler * handler = ( SP_CompletionHandler * ) ((void**)arg)[0];
	SP_Message * msg = ( SP_Message * ) ((void**)arg)[ 1 ];

	handler->completionMessage( msg );
*/
	free( arg );
}

int TcpServer :: start()
{
	/* Don't die with SIGPIPE on remote read shutdown. That's dumb. */
	signal( SIGPIPE, SIG_IGN );

	int ret = 0;
	int listenFD = -1;

	ret = IOUtils::tcpListen( mBindIP, mPort, &listenFD, 0 );

	if( 0 == ret ) {

		EventArg eventArg( mTimeout );

		// Clean close on SIGINT or SIGTERM.
		struct event evSigInt, evSigTerm;
		signal_set( &evSigInt, SIGINT,  sigHandler, this );
		event_base_set( eventArg.getEventBase(), &evSigInt );
		signal_add( &evSigInt, NULL);

		signal_set( &evSigTerm, SIGTERM, sigHandler, this );
		event_base_set( eventArg.getEventBase(), &evSigTerm );
		signal_add( &evSigTerm, NULL);


		AcceptArg_t acceptArg;
		memset( &acceptArg, 0, sizeof( AcceptArg_t ) );

		acceptArg.mEventArg 		= &eventArg;
		acceptArg.mReqQueueSize 	= mReqQueueSize;
		acceptArg.mMaxConnections 	= mMaxConnections;
		acceptArg.mRefusedMsg 		= mRefusedMsg;

		struct event evAccept;
		event_set( &evAccept, listenFD, EV_READ|EV_PERSIST, EventCall::onAccept, &acceptArg );
		event_base_set( eventArg.getEventBase(), &evAccept );
		event_add( &evAccept, NULL );

		//SP_Executor workerExecutor( mMaxThreads, "work" );
		//SP_Executor actExecutor( 1, "act" );
		//SP_CompletionHandler * completionHandler = mHandlerFactory->createCompletionHandler();

		/* Start the event loop. */
		while( 0 == mIsShutdown ) {
			event_base_loop( eventArg.getEventBase(), EVLOOP_ONCE );
/*
			for( ; NULL != eventArg.getInputResultQueue()->top(); ) {
				SP_Task * task = (SP_Task*)eventArg.getInputResultQueue()->pop();
				workerExecutor.execute( task );
			}

			for( ; NULL != eventArg.getOutputResultQueue()->top(); ) {
				SP_Message * msg = (SP_Message*)eventArg.getOutputResultQueue()->pop();

				void ** arg = ( void** )malloc( sizeof( void * ) * 2 );
				arg[ 0 ] = (void*)completionHandler;
				arg[ 1 ] = (void*)msg;

				actExecutor.execute( outputCompleted, arg );
			}*/
		}

		//delete completionHandler;

		syslog( LOG_NOTICE, "Server is shutdown." );

		event_del( &evAccept );

		signal_del( &evSigTerm );
		signal_del( &evSigInt );

		close( listenFD );
	}

	return ret;
}

