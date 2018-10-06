

#include <sys/socket.h>
#include <netinet/in.h>

#include "TaskVideoSend.hpp"
#include "BufferCache.hpp"


#include "common.h"




#ifdef 	__ANDROID__
#define	FILE_PATH	"/sdcard/camera_640x480.h264"
#else
#define	FILE_PATH	"h264/camera_640x480.h264"
#endif

	TaskVideoSend::TaskVideoSend( Sid_t sid )
				:mPackHeadLen(sizeof(PACK_HEAD))
				,TaskBase(sid)
				,mRecvDataLen(0)
				,mRecvHeadLen(0)
				,mTotalLen(0)
				,mSeqid(0)
	{
		GLOGD("TaskVideoSend filename:%s.", FILE_PATH);
		memset(mReadBuff, 1500, 0);
		mInBuffer = new BufferCache();
		mwFile = OpenBitstreamFile( FILE_PATH ); //camera_640x480.h264 //camera_1280x720.h264
		if(mwFile==NULL) {
			GLOGE("open file:%s failed.", FILE_PATH);
		}
		mNALU  = AllocNALU(8000000);

		tpcSendMsg(VIDEO_RECV_MSG);
	}


	TaskVideoSend::~TaskVideoSend() {
		delete mInBuffer;
		mInBuffer = NULL;

		if(mwFile != NULL)
			fclose(mwFile);

		FreeNALU(mNALU);
	}


	int TaskVideoSend::StartTask() {
		return 0;
	}

	int TaskVideoSend::StopTask() {
		return 0;
	}

	int TaskVideoSend::sendEx(char*data,int len) {
		int leftLen = len, iRet = 0;
		fd_set fdSend;
		struct timeval timeout;
		int sockId = mSid.mKey;
		do {
			if(leftLen<=MAX_MTU)
				iRet = send(sockId, data+len-leftLen, leftLen, 0);
			else
				iRet = send(sockId, data+len-leftLen, MAX_MTU, 0);
			if(iRet<0)
				GLOGE("send data error ret:%d.", iRet);
			leftLen -= iRet;
		}while(leftLen>0);

		return len - leftLen;
	}

	void TaskVideoSend::packetHead(int fid, short pid, int len, unsigned char type, LPPACK_HEAD lpPack) {
		memset(lpPack, 0, sizeof(PACK_HEAD));
		lpPack->type 		= type;
		lpPack->fid 		= htonl(fid);
		lpPack->pid			= htons(pid);
		lpPack->len 		= htonl(len);
	}

	int TaskVideoSend::tpcSendMsg(unsigned char msg) {
		int iRet = 0;
		char sendData[1500] = {0};
		packetHead(0, 0, mPackHeadLen, msg, (LPPACK_HEAD)sendData);
		iRet = sendEx(sendData, mPackHeadLen);
		return iRet;
	}

	int TaskVideoSend::tcpSendData(char*data, int len) {
		int iRet = 0;
		char sendData[1500] = {0};
		mSeqid++;
		packetHead(mSeqid, 0, len, VIDEO_RECV_STREAM, (LPPACK_HEAD)sendData);
		iRet = sendEx(sendData, mPackHeadLen);
		iRet = sendEx(data, len);
		return iRet;
	}

	int TaskVideoSend::readBuffer() {
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

	int TaskVideoSend::writeBuffer() {
		int ret = 0;
		if(feof(mwFile)) {
			//mRunning = false;
			GLOGW("read file done.");
			return 0;
		}
		int size=GetAnnexbNALU(mwFile, mNALU);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
		//GLOGE("GetAnnexbNALU size:%d", n->len);
		if(size<4)
		{
			GLOGE("get nul error!\n");
			//continue;
		}
		ret = tcpSendData((char*)mNALU->buf, mNALU->len);

		return ret;
	}
