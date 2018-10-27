


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "TaskVideoRecv.hpp"
#include "TaskVideoSend.hpp"
#include "TaskPlayback.hpp"
#include "Session.hpp"


#include "config.h"
#include "event.h"

#include "h264.h"
#include "basedef.h"
#include "protocol.h"
#include "net_protocol.h"


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
			:mHeadLenConst(sizeof(NET_HEAD))
			,mSid(sid)
			,mTaskBase(NULL)
			,mArg(NULL)
			,mTotalDataLen(0)
			,mHasRecvLen(0)
			,mbRecvHead(true)
{
	mReadEvent  = (struct event*)malloc( sizeof( struct event ) );
	mWriteEvent = (struct event*)malloc( sizeof( struct event ) );

	mStatus  	= eNormal;
	mRunning 	= 0;
	mWriting 	= 0;
	mReading 	= 0;
}

Session :: Session( Sid_t sid, short type)
			:mHeadLenConst(sizeof(NET_HEAD))
			,mSid(sid)
			,mTaskBase(NULL)
			,mArg(NULL)
			,mTotalDataLen(0)
			,mHasRecvLen(0)
			,mbRecvHead(true)
{
	mReadEvent  = (struct event*)malloc( sizeof( struct event ) );
	mWriteEvent = (struct event*)malloc( sizeof( struct event ) );

	mStatus  	= eNormal;
	mRunning 	= 0;
	mWriting 	= 0;
	mReading 	= 0;
	switch(type) {
		case VIDEO_SEND_MSG:
			mTaskBase = new TaskVideoSend( mSid );
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
	return recvCount;
}

int Session :: readBuffer() {
	int ret = 0;

	if(mTaskBase!=NULL) {
		ret = mTaskBase->readBuffer();
	}
	else
	{
//		ret = recv(mSid.mKey, mReadBuff+mHasRecvLen, 1500, 0);
//		mHasRecvLen+=ret;
//		LPNET_CMD pCmdbuf = (LPNET_CMD)mReadBuff;
//		GLOGE("Session flag:%08x ret:%d data:%s", pCmdbuf->dwFlag, ret, pCmdbuf->lpData);

		if(mbRecvHead) {
			ret = recv(mSid.mKey, mReadBuff+mHasRecvLen, mHeadLenConst-mHasRecvLen, 0);
			if(ret>0) {
				mHasRecvLen+=ret;
				if(mHasRecvLen==mHeadLenConst) {

					LPNET_HEAD head = (LPNET_HEAD)mReadBuff;
					mTotalDataLen = head->dwLength;
					mHasRecvLen = 0;
					mbRecvHead = false;

					GLOGE("Session flag:%08x frameLen:%d ret:%d", head->dwFlag, mTotalDataLen, ret);
					//GLOGE("Session flag:%08x ret:%d data:%s", cmdbuf->dwFlag, ret, cmdbuf->lpData);
					ret = recvPackData();
				}
			}
		}else{
			ret = recvPackData();
		}
	}

	return ret;
}

int Session :: writeBuffer() {
	int ret = 0;
	if(mTaskBase!=NULL) {
		ret = mTaskBase->writeBuffer();
	}
	return ret;
}

int Session ::recvPackData() {
	int ret = recv(mSid.mKey, mReadBuff+mHeadLenConst+mHasRecvLen, mTotalDataLen-mHasRecvLen, 0);
	if(ret>0) {
		mHasRecvLen+=ret;
		if(mHasRecvLen==mTotalDataLen) {

		    int lValueLen;
		    char *acValue = new char[256];
		    memset(acValue,0, 256);
			LPNET_CMD pCmdbuf = (LPNET_CMD)mReadBuff;
		    PROTO_GetValueByName(mReadBuff, "play path", (char**)&acValue, &lValueLen);
		    GLOGE("filename:%s",acValue);

		    if(access(acValue, F_OK)!=0)
		    	GLOGE("filename %s is no exist.",acValue);

			short type = pCmdbuf->dwIndex;
			switch(type) {
				case 0:
					mTaskBase = new TaskPlayback( mSid, acValue );
					break;

				case VIDEO_RECV_MSG:
					mTaskBase = new TaskVideoRecv( mSid );
					break;

				case FILE_RECV_MSG:
					break;
			}
			mTotalDataLen = 0;
			mHasRecvLen   = 0;
			mbRecvHead    = true;

			SAFE_DELETE(acValue);
			//GLOGE("Session flag:%08x frameLen:%d ret:%d", pCmdbuf->dwFlag, pCmdbuf->dwLength, ret);
			//GLOGE("Session flag:%08x ret:%d data:%s", pCmdbuf->dwFlag, ret, pCmdbuf->lpData);
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



