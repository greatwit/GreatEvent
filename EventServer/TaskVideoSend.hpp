#ifndef __TaskVideoSend_hpp__
#define __TaskVideoSend_hpp__

#include <stdio.h>
#include <unistd.h>

#include "h264.h"
#include "TaskBase.hpp"

class BufferCache;

class TaskVideoSend :public TaskBase {
public:
	TaskVideoSend( Sid_t sid );
	virtual ~TaskVideoSend();
	virtual int StartTask();
	virtual int StopTask();
	virtual int readBuffer();
	virtual int writeBuffer();

	void packetHead(int fid, short pid, int len, unsigned char type, LPPACK_HEAD lpPack);
	int tcpSendData(char*data, int len);
	int tpcSendMsg(unsigned char msg);
	int sendEx(char*data,int len);

private:
	char mReadBuff[1500];
	BufferCache * mInBuffer;
	FILE			*mwFile;
	NALU_t 			*mNALU;
	int				mSeqid;

	int mPackHeadLen;

	int  mRecvDataLen;
	int  mRecvHeadLen;
	int  mTotalLen;
};


#endif
