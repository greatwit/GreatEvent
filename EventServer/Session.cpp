


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "TaskVideoRecv.hpp"
#include "Session.hpp"
#include "sputils.hpp"

#include "config.h"
#include "event.h"

#include "h264.h"
#include "common.h"



//---------------------------sessionManager----------------------------------------

typedef struct tagSessionEntry {
	uint16_t mSeq;
	Session  *mSession;
} SessionEntry;

SessionManager :: SessionManager()
{
	mCount = 0;
	memset( mArray, 0, sizeof( mArray ) );
}

SessionManager :: ~SessionManager()
{
	for( int i = 0; i < (int)( sizeof( mArray ) / sizeof( mArray[0] ) ); i++ ) {
		SessionEntry_t * list = mArray[ i ];
		if( NULL != list ) {
			SessionEntry_t * iter = list;
			for( int i = 0; i < 1024; i++, iter++ ) {
				if( NULL != iter->mSession ) {
					delete iter->mSession;
					iter->mSession = NULL;
				}
			}
			free( list );
		}
	}

	memset( mArray, 0, sizeof( mArray ) );
}

int SessionManager :: getCount()
{
	return mCount;
}

void SessionManager :: put( uint16_t key, Session * session, uint16_t * seq )
{
	int row = key / 1024, col = key % 1024;

	if( NULL == mArray[ row ] ) {
		mArray[ row ] = ( SessionEntry_t * )calloc(
			1024, sizeof( SessionEntry_t ) );
	}

	SessionEntry_t * list = mArray[ row ];
	list[ col ].mSession = session;
	*seq = list[ col ].mSeq;

	mCount++;
}

Session * SessionManager :: get( uint16_t key, uint16_t * seq )
{
	int row = key / 1024, col = key % 1024;

	Session * ret = NULL;

	SessionEntry_t * list = mArray[ row ];
	if( NULL != list ) {
		ret = list[ col ].mSession;
		* seq = list[ col ].mSeq;
	} else {
		* seq = 0;
	}

	return ret;
}

Session * SessionManager :: remove( uint16_t key, uint16_t * seq )
{
	int row = key / 1024, col = key % 1024;

	Session * ret = NULL;

	SessionEntry_t * list = mArray[ row ];
	if( NULL != list ) {
		ret = list[ col ].mSession;
		if( NULL != seq ) * seq = list[ col ].mSeq;

		list[ col ].mSession = NULL;
		list[ col ].mSeq++;

		mCount--;
	}

	return ret;
}



//----------------------------------session---------------------------------

Session :: Session( Sid_t sid )
			:mPackHeadLen(sizeof(PACK_HEAD))
			,mSid(sid)
			,mTaskBase(NULL)
			,mArg(NULL)
			,mRecvDataLen(0)
			,mRecvHeadLen(0)
			,mTotalLen(0)
{
	mReadEvent  = (struct event*)malloc( sizeof( struct event ) );
	mWriteEvent = (struct event*)malloc( sizeof( struct event ) );

	mStatus  	= eNormal;
	mRunning 	= 0;
	mWriting 	= 0;
	mReading 	= 0;
}

Session :: Session( Sid_t sid, short type)
			:mPackHeadLen(sizeof(PACK_HEAD))
			,mSid(sid)
			,mTaskBase(NULL)
			,mArg(NULL)
			,mRecvDataLen(0)
			,mRecvHeadLen(0)
			,mTotalLen(0)
{
	mReadEvent  = (struct event*)malloc( sizeof( struct event ) );
	mWriteEvent = (struct event*)malloc( sizeof( struct event ) );

	mStatus  	= eNormal;
	mRunning 	= 0;
	mWriting 	= 0;
	mReading 	= 0;
	switch(type) {
		case VIDEO_SEND_MSG:
			mTaskBase = new TaskVideoRecv( mSid );
			break;
		case FILE_SEND_MSG:
			break;
	}
}

Session :: ~Session()
{
	free( mReadEvent );
	mReadEvent = NULL;

	free( mWriteEvent );
	mWriteEvent = NULL;

	if(mTaskBase!=NULL) {
		delete mTaskBase;
		mTaskBase = NULL;
	}
}

int Session :: recvEx(char*pData, int len) {
	int recvCount = 0, recvRet = 0;
	do{
		recvRet = recv(mSid.mKey, pData+recvCount, len-recvCount, 0);
		if(recvRet<=0)
			return recvRet;

		recvCount += recvRet;
	}while(recvCount<len);
}

int Session :: readBuffer() {
	int ret = 0;

	if(mTaskBase!=NULL) {
		ret = mTaskBase->readBuffer();
	}
	else
	{
		int leftLent = mTotalLen - mRecvDataLen;
		ret = recv(mSid.mKey, mReadBuff+mRecvHeadLen, mPackHeadLen-mRecvHeadLen, 0);
		if(ret>0) {
			mRecvHeadLen+=ret;
			if(mRecvHeadLen==mPackHeadLen) {

				LPPACK_HEAD head = (LPPACK_HEAD)mReadBuff;
				short type = head->type;
				switch(type) {
					case VIDEO_RECV_MSG:
						mTaskBase = new TaskVideoRecv( mSid );
						break;
					case FILE_RECV_MSG:
						break;
				}
				mRecvHeadLen = 0;
				GLOGE("Session fid:%d frameLen:%d ret:%d", htonl(head->fid), mTotalLen, ret);
			}
		}
	}

	return ret;
}

struct event * Session :: getReadEvent()
{
	return mReadEvent;
}

struct event * Session :: getWriteEvent()
{
	return mWriteEvent;
}

void Session :: setArg( void * arg )
{
	mArg = arg;
}

void * Session :: getArg()
{
	return mArg;
}

Sid_t Session :: getSid()
{
	return mSid;
}

void Session :: setStatus( int status )
{
	mStatus = status;
}

int Session :: getStatus()
{
	return mStatus;
}

int Session :: getRunning()
{
	return mRunning;
}

void Session :: setRunning( int running )
{
	mRunning = running;
}

int Session :: getWriting()
{
	return mWriting;
}

void Session :: setWriting( int writing )
{
	mWriting = writing;
}

int Session :: getReading()
{
	return mReading;
}

void Session :: setReading( int reading )
{
	mReading = reading;
}



