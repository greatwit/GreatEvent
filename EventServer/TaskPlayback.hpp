#ifndef __TaskPlayback_hpp__
#define __TaskPlayback_hpp__

#include <stdio.h>
#include <unistd.h>

#include "h264.h"
#include "basedef.h"
#include "TaskBase.hpp"
#include "FfmpegContext.h"

class BufferCache;



class TaskPlayback :public TaskBase {

public:
	TaskPlayback( Session*sess, Sid_t& sid, char*filename);
	virtual ~TaskPlayback();
	virtual int StartTask();
	virtual int StopTask();
	virtual int readBuffer();
	virtual int writeBuffer();

	void packetHead(int fid, short pid, int len, unsigned char type, LPPACK_HEAD lpPack);
	int tcpSendData(char*data, int len);
	int tpcSendMsg(unsigned char msg);
	int sendEx(char*data,int len);
	int getLoginRet(LOGIN_RET&lRet);

private:
	int recvPackData();

	FfmpegContext 		*mFfmpeg;
	struct tagRecvBuffer mRecvBuffer;
	struct tagSendBuffer mSendBuffer;
	BufferCache 		* mInBuffer;
	FILE				*mpFile;
	NALU_t 				*mNALU;
	int					mSeqid;
	Session				*mSess;

	const int 			mPackHeadLen;
};


#endif
