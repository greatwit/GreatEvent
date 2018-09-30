

#include <sys/socket.h>
#include <netinet/in.h>

#include "TaskFileRecv.hpp"
#include "BufferCache.hpp"

#include "h264.h"
#include "common.h"

#ifdef 	__ANDROID__
#define	FILE_PATH	"/sdcard/w.h264"
#else
#define	FILE_PATH	"w.h264"
#endif

	TaskFileRecv::TaskFileRecv( Sid_t sid )
				:mPackHeadLen(sizeof(PACK_HEAD))
				,TaskBase(sid)
				,mRecvDataLen(0)
				,mRecvHeadLen(0)
				,mTotalLen(0)
	{
		memset(mReadBuff, 1500, 0);
		mInBuffer = new BufferCache();
		mwFile = fopen(FILE_PATH, "w");
	}


	TaskFileRecv::~TaskFileRecv() {
		delete mInBuffer;
		mInBuffer = NULL;

		if(mwFile != NULL)
			fclose(mwFile);
	}


	int TaskFileRecv::StartTask() {
		return 0;
	}

	int TaskFileRecv::StopTask() {
		return 0;
	}

	int TaskFileRecv::readBuffer() {
		int ret = 0;

		return ret;
	}

	int TaskFileRecv::writeBuffer() {
		return 0;
	}


