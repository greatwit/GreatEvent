

#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>


#include "ActorStation.hpp"
#include "common.h"

	ActorStation :: ActorStation( )
		:mIsShutdown(0)
		,mIsRunning(0)
		,mTimeout(60) {
		mEventArg.setTimeout(mTimeout);
	}

	ActorStation :: ~ActorStation() {

	}

	void ActorStation :: setTimeout( int timeout )
	{
		mTimeout = timeout > 0 ? timeout : mTimeout;
	}

	const EventArg&  ActorStation :: getEventArg() {
		return mEventArg;
	}

	void ActorStation :: shutdown() {
		mIsShutdown = 1;
	}

	int ActorStation :: isRunning() {
		return mIsRunning;
	}

	int ActorStation :: run() {
		int ret = -1;

		pthread_attr_t attr;
		pthread_attr_init( &attr );
		assert( pthread_attr_setstacksize( &attr, 1024 * 1024 ) == 0 );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

		pthread_t thread = 0;
		ret = pthread_create( &thread, &attr, reinterpret_cast<void*(*)(void*)>(eventLoop), this );
		pthread_attr_destroy( &attr );
		if( 0 == ret ) {
			GLOGE( "Thread #%ld has been created to listen.", thread );
		} else {
			mIsRunning = 0;
			GLOGE( "Unable to create a thread for TCP server, %s", strerror( errno ) ) ;
		}

		return ret;
	}

	void ActorStation :: runForever() {
		run();
	}

	void * ActorStation :: eventLoop( void * arg ) {
		ActorStation * server = (ActorStation*)arg;

		server->mIsRunning = 1;

		server->start();

		server->mIsRunning = 0;

		return NULL;
	}

	int ActorStation :: start() {
		/* Don't die with SIGPIPE on remote read shutdown. That's dumb. */
		signal( SIGPIPE, SIG_IGN );

		int ret = 0;

		if( 0 == ret ) {
			// Clean close on SIGINT or SIGTERM.
			struct event evSigInt, evSigTerm;
			signal_set( &evSigInt, SIGINT,  sigHandler, this );
			event_base_set( mEventArg.getEventBase(), &evSigInt );
			signal_add( &evSigInt, NULL);

			signal_set( &evSigTerm, SIGTERM, sigHandler, this );
			event_base_set( mEventArg.getEventBase(), &evSigTerm );
			signal_add( &evSigTerm, NULL);


			/* Start the event loop. */
			while( 0 == mIsShutdown ) {
				event_base_loop( mEventArg.getEventBase(), EVLOOP_ONCE );
			}


			printf("Server is shutdown.\n");

			signal_del( &evSigTerm );
			signal_del( &evSigInt );
		}
		return ret;
	}

	void ActorStation :: sigHandler( int, short, void * arg ) {
		ActorStation * server = (ActorStation*)arg;
		server->shutdown();
	}

