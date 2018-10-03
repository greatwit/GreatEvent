


#ifndef __session_hpp__
#define __session_hpp__



#include <unistd.h>
#include "TaskBase.hpp"

//class SP_Handler;

class SP_ArrayList;


struct event;


class Session {
public:
	Session( Sid_t sid );
	Session( Sid_t sid, short type);
	virtual ~Session();

	struct event * getReadEvent();	
	struct event * getWriteEvent();	

	//void setHandler( SP_Handler * handler );
	//SP_Handler * getHandler();

	void setArg( void * arg );
	void * getArg();

	Sid_t getSid();

	enum { eNormal, eWouldExit, eExit };
	void setStatus( int status );
	int getStatus();

	int readBuffer();
	int recvEx(char*pData, int len);
	int writeBuffer();

	int getRunning();
	void setRunning( int running );

	int getReading();
	void setReading( int reading );

	int getWriting();
	void setWriting( int writing );

private:
	Session( Session & );
	Session & operator=( Session & );

	Sid_t mSid;
	TaskBase *mTaskBase;

	struct event * mReadEvent;
	struct event * mWriteEvent;

	//SP_Handler * mHandler;
	void * mArg;


	char mStatus;
	char mRunning;
	char mWriting;
	char mReading;

	int mPackHeadLen;

	int  mRecvDataLen;
	int  mRecvHeadLen;
	int  mTotalLen;
	char mReadBuff[1500];
};

typedef struct tagSessionEntry SessionEntry_t;

class SessionManager {
public:
	SessionManager();
	~SessionManager();

	int getCount();
	void put( uint16_t key, Session * session, uint16_t * seq );
	Session * get( uint16_t key, uint16_t * seq );
	Session * remove( uint16_t key, uint16_t * seq = NULL );

private:
	int mCount;
	SessionEntry_t * mArray[ 64 ];
};

#endif

