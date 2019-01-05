

#include <sys/socket.h>
#include <netinet/in.h>

#include "TaskVideoRecv.hpp"
#include "BufferCache.hpp"

#include "h264.h"
#include "basedef.h"

#ifdef 	__ANDROID__
#define	FILE_PATH	"/sdcard/w.h264"
#else
#define	FILE_PATH	"w.h264"
#endif

	TaskVideoRecv::TaskVideoRecv( Session*sess, Sid_t &sid )
				:mPackHeadLen(sizeof(PACK_HEAD))
				,TaskBase(sid)
				,mSess(sess)
				,mRecvDataLen(0)
				,mRecvHeadLen(0)
				,mTotalLen(0)
	{
		memset(mReadBuff, 1500, 0);
		mInBuffer = new BufferCache();
		mwFile = fopen(FILE_PATH, "w");
		if(mwFile==NULL)
			GLOGE("fopen filename:%s failed.", FILE_PATH);
	}

	TaskVideoRecv::~TaskVideoRecv() {
		delete mInBuffer;
		mInBuffer = NULL;

		if(mwFile != NULL)
			fclose(mwFile);
	}

	int TaskVideoRecv::StartTask() {
		return 0;
	}

	int TaskVideoRecv::StopTask() {
		return 0;
	}

	int TaskVideoRecv::readBuffer() {
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
					//GLOGW("frameLen:%d buffsize:%d\n", mTotalLen, mInBuffer->getSize());
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

	int TaskVideoRecv::writeBuffer() {
		GLOGE("TaskVideoRecv write event\n");
		return 0;
	}


