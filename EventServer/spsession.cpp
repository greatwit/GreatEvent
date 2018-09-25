/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "spsession.hpp"
#include "spbuffer.hpp"
#include "sputils.hpp"
#include "spioutils.hpp"

#include "config.h"
#include "event.h"
#include "h264.h"


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
{
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

	mwFile = fopen("w.h264", "w");
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

int SP_Session :: readBuffer(){
	int ret = 0;
	char readBuff[1500]={0};
	ret = recv(mSid.mKey, readBuff, MAX_MTU, 0);
	if(ret>0)
	{
		LPPACK_HEAD head = (LPPACK_HEAD)readBuff;
		mInBuffer->append(readBuff+mPackHeadLen, ret - mPackHeadLen);
		if(head->type.M != 0)
		{
			int frameLen = htonl(head->len);
			int allPackLen = mInBuffer->getSize() ;//completed first package head
			char tag[4] = {0x00, 0x00, 0x00, 0x01};
			if(frameLen == allPackLen)
			{
				fwrite(tag, 1, 4, mwFile);
				fwrite(mInBuffer->getBuffer(), 1, mInBuffer->getSize(), mwFile);
				//printf("buffer size:%d\n", mInBuffer->getSize());
				//printf("fid:%d pid:%d len:%d buffsize:%d\n", htonl(head->fid), htons(head->pid), htonl(head->len) - mPackHeadLen, mInBuffer->getSize());
			}
			else if(allPackLen > frameLen)
			{
				printf("packLen:%d bufflen:%d\n", frameLen, mInBuffer->getSize());
				int dataIndex 	= 0;
				const char *buff 	= (const char *)mInBuffer->getBuffer();
				while(allPackLen>0) {
					fwrite(tag, 1, 4, mwFile);
					fwrite(buff+dataIndex, 1, frameLen, mwFile);
					mInBuffer->erase(frameLen+dataIndex);
					allPackLen -= frameLen;
					printf("fid:%d pid:%d len:%d buffsize:%d\n", htonl(head->fid), htons(head->pid), htonl(head->len) - mPackHeadLen, mInBuffer->getSize());
					if(allPackLen<=0)
						break;

					buff	= (const char *)mInBuffer->getBuffer();
					head 	= (LPPACK_HEAD)(buff);
					frameLen =  htonl(head->len);
					printf("next packLen:%d\n", frameLen);

					dataIndex = mPackHeadLen;
					if(head->type.M == 0) {
						printf("mark is 0. \n");
						//return ret;
					}
				}
			}
			mInBuffer->reset();
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



