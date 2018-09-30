#ifndef __TaskFileRecv_hpp__
#define __TaskFileRecv_hpp__


#include <stdio.h>

#include "TaskBase.hpp"

class BufferCache;

class TaskFileRecv :public TaskBase {
public:
	TaskFileRecv( Sid_t sid );
	virtual ~TaskFileRecv();
	virtual int StartTask();
	virtual int StopTask();
	virtual int readBuffer();
	virtual int writeBuffer();

private:
	char mReadBuff[1500];
	BufferCache * mInBuffer;
	FILE			*mwFile;

	int mPackHeadLen;

	int  mRecvDataLen;
	int  mRecvHeadLen;
	int  mTotalLen;
};


#endif
