/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "spsession.hpp"
#include "spbuffer.hpp"
#include "sputils.hpp"
#include "spioutils.hpp"

#include "config.h"
#include "event.h"

#include "h264.h"
#include "common.h"

#ifdef 	__ANDROID__
#define	FILE_PATH	"/sdcard/w.h264"
#else
#define	FILE_PATH	"w.h264"
#endif

//---------------------------sessionManager----------------------------------------

typedef struct tagSP_SessionEntry {
	uint16_t mSeq;
	SP_Session * mSession;
} SP_SessionEntry;

SP_SessionManager :: SP_SessionManager()
{
	mCount = 0;
	memset( mArray, 0, sizeof( mArray ) );
}

SP_SessionManager :: ~SP_SessionManager()
{
	for( int i = 0; i < (int)( sizeof( mArray ) / sizeof( mArray[0] ) ); i++ ) {
		SP_SessionEntry_t * list = mArray[ i ];
		if( NULL != list ) {
			SP_SessionEntry_t * iter = list;
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

int SP_SessionManager :: getCount()
{
	return mCount;
}

void SP_SessionManager :: put( uint16_t key, SP_Session * session, uint16_t * seq )
{
	int row = key / 1024, col = key % 1024;

	if( NULL == mArray[ row ] ) {
		mArray[ row ] = ( SP_SessionEntry_t * )calloc(
			1024, sizeof( SP_SessionEntry_t ) );
	}

	SP_SessionEntry_t * list = mArray[ row ];
	list[ col ].mSession = session;
	*seq = list[ col ].mSeq;

	mCount++;
}

SP_Session * SP_SessionManager :: get( uint16_t key, uint16_t * seq )
{
	int row = key / 1024, col = key % 1024;

	SP_Session * ret = NULL;

	SP_SessionEntry_t * list = mArray[ row ];
	if( NULL != list ) {
		ret = list[ col ].mSession;
		* seq = list[ col ].mSeq;
	} else {
		* seq = 0;
	}

	return ret;
}

SP_Session * SP_SessionManager :: remove( uint16_t key, uint16_t * seq )
{
	int row = key / 1024, col = key % 1024;

	SP_Session * ret = NULL;

	SP_SessionEntry_t * list = mArray[ row ];
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

SP_Session :: SP_Session( SP_Sid_t sid )
			:mPackHeadLen(sizeof(PACK_HEAD))
			,mSid(sid)
			,mRecvDataLen(0)
			,mRecvHeadLen(0)
			,mTotalLen(0)
{
	memset(mReadBuff, 1500, 0);
	mReadEvent  = (struct event*)malloc( sizeof( struct event ) );
	mWriteEvent = (struct event*)malloc( sizeof( struct event ) );

	//mHandler = NULL;
	mArg = NULL;

	mInBuffer = new SP_Buffer();

	mOutOffset = 0;
	mOutList = new SP_ArrayList();

	mStatus  = eNormal;
	mRunning = 0;
	mWriting = 0;
	mReading = 0;

	mwFile = fopen(FILE_PATH, "w");
}

SP_Session :: ~SP_Session()
{
	free( mReadEvent );
	mReadEvent = NULL;

	free( mWriteEvent );
	mWriteEvent = NULL;

	delete mInBuffer;
	mInBuffer = NULL;

	delete mOutList;
	mOutList = NULL;

	if(mwFile != NULL)
		fclose(mwFile);
}

int SP_Session :: recvEx(char*pData, int len) {
	int recvCount = 0, recvRet = 0;
	do{
		recvRet = recv(mSid.mKey, pData+recvCount, len-recvCount, 0);
		if(recvRet<=0)
			return recvRet;
		recvCount += recvRet;
	}while(recvCount<len);
}

int SP_Session :: readBuffer() {
	int ret = 0;
	int leftLent = mTotalLen - mRecvDataLen;
	if(leftLent>0) {

		if(leftLent < MAX_MTU)
			ret = recv(mSid.mKey, mReadBuff, leftLent, 0);
		else
			ret = recv(mSid.mKey, mReadBuff, MAX_MTU, 0);

		if(ret>0 ) {
			mRecvDataLen += ret;
			mInBuffer->append(mReadBuff, ret);

			if(mRecvDataLen == mTotalLen) {
				//printf("frameLen:%d buffsize:%d\n", mTotalLen, mInBuffer->getSize());
				char tag[4] = {0x00, 0x00, 0x00, 0x01};
				fwrite(tag, 1, 4, mwFile);
				fwrite(mInBuffer->getBuffer(), 1, mInBuffer->getSize(), mwFile);
				mRecvDataLen  	= 0;
				mTotalLen 		= 0;
				mInBuffer->reset();
			}
		}
	}
	else
	{
		ret = recv(mSid.mKey, mReadBuff+mRecvHeadLen, mPackHeadLen-mRecvHeadLen, 0);
		if(ret>0) {
			mRecvHeadLen+=ret;
			if(mRecvHeadLen==mPackHeadLen) {
				LPPACK_HEAD head = (LPPACK_HEAD)mReadBuff;
				mTotalLen = htonl(head->len);
				mRecvHeadLen = 0;
				GLOGE("---fid:%d frameLen:%d ret:%d", htonl(head->fid), mTotalLen, ret);
			}
		}
	}

	return ret;
}

struct event * SP_Session :: getReadEvent()
{
	return mReadEvent;
}

struct event * SP_Session :: getWriteEvent()
{
	return mWriteEvent;
}

void SP_Session :: setArg( void * arg )
{
	mArg = arg;
}

void * SP_Session :: getArg()
{
	return mArg;
}

SP_Sid_t SP_Session :: getSid()
{
	return mSid;
}

SP_Buffer * SP_Session :: getInBuffer()
{
	return mInBuffer;
}

void SP_Session :: setOutOffset( int offset )
{
	mOutOffset = offset;
}

int SP_Session :: getOutOffset()
{
	return mOutOffset;
}

SP_ArrayList * SP_Session :: getOutList()
{
	return mOutList;
}

void SP_Session :: setStatus( int status )
{
	mStatus = status;
}

int SP_Session :: getStatus()
{
	return mStatus;
}

int SP_Session :: getRunning()
{
	return mRunning;
}

void SP_Session :: setRunning( int running )
{
	mRunning = running;
}

int SP_Session :: getWriting()
{
	return mWriting;
}

void SP_Session :: setWriting( int writing )
{
	mWriting = writing;
}

int SP_Session :: getReading()
{
	return mReading;
}

void SP_Session :: setReading( int reading )
{
	mReading = reading;
}



