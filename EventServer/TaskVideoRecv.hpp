#ifndef __TaskVideoRecv_hpp__
#define __TaskVideoRecv_hpp__


#include <stdio.h>

#include "TaskBase.hpp"

class BufferCache;

class TaskVideoRecv :public TaskBase {

public:
	TaskVideoRecv( Session*sess, Sid_t &sid );
	TaskVideoRecv( Session*sess, Sid_t &sid, char*filepath );
	virtual ~TaskVideoRecv();
	virtual int StartTask();
	virtual int StopTask();
	virtual int readBuffer();
	virtual int writeBuffer();

private:
	char mReadBuff[1500];
	BufferCache 	*mInBuffer;
	FILE			*mwFile;
	Session			*mSess;

	int mPackHeadLen;

	int  mRecvDataLen;
	int  mRecvHeadLen;
	int  mTotalLen;
};


#endif
