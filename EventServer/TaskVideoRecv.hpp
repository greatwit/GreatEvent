#ifndef __TaskVideoRecv_hpp__
#define __TaskVideoRecv_hpp__


#include <stdio.h>

#include "TaskBase.hpp"

class BufferCache;

class TaskVideoRecv :public TaskBase {

public:
	TaskVideoRecv( Sid_t sid );
	virtual ~TaskVideoRecv();
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
