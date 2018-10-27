

#include <sys/socket.h>
#include <netinet/in.h>

#include "TaskPlayback.hpp"
#include "BufferCache.hpp"


#include "basedef.h"
#include "protocol.h"

#define	FILE_PATH	"h264/camera_640x480.h264"

	TaskPlayback::TaskPlayback( Sid_t sid, char*filename )
				:mPackHeadLen(sizeof(NET_HEAD))
				,TaskBase(sid)
				,mFfmpeg(NULL)
				,mpFile(NULL)
				,mRecvDataLen(0)
				,mRecvHeadLen(0)
				,mTotalLen(0)
				,mSeqid(0)
	{
		//GLOGD("TaskPlayback filename:%s.", FILE_PATH);
		memset(mReadBuff, 1500, 0);
		mInBuffer = new BufferCache();

		mFfmpeg = new FfmpegContext(filename);

		char lpRet[sizeof(NET_CMD) + sizeof(LOGIN_RET)];
		ZeroMemory(lpRet,sizeof(lpRet));
		getLoginRes(lpRet);
		int ret = sendEx(lpRet, sizeof(NET_CMD) + sizeof(LOGIN_RET));

//		mpFile = OpenBitstreamFile( filename ); //camera_640x480.h264 //camera_1280x720.h264
//		if(mpFile==NULL) {
//			GLOGE("open file:%s failed.", filename);
//		}
		mNALU  = AllocNALU(8000000);
//
//		tpcSendMsg(VIDEO_RECV_MSG);

		GLOGE("net_cmd len:%d send ret:%d",sizeof(NET_CMD) + sizeof(LOGIN_RET), ret);
	}


	TaskPlayback::~TaskPlayback() {
		delete mInBuffer;
		mInBuffer = NULL;

		if(mpFile != NULL)
			fclose(mpFile);

		FreeNALU(mNALU);

		if(mFfmpeg!=NULL) {
			delete mFfmpeg;
			mFfmpeg = NULL;
		}
	}


	int TaskPlayback::StartTask() {
		char lpRet[sizeof(NET_CMD) + sizeof(LOGIN_RET)];
		ZeroMemory(lpRet, sizeof(lpRet));
		((LPNET_CMD)lpRet)->dwFlag = NET_FLAG;
		((LPNET_CMD)lpRet)->dwCmd  = MODULE_MSG_LOGINRET;
		((LPNET_CMD)lpRet)->dwLength = sizeof(LOGIN_RET);
		LPLOGIN_RET lpLR = (LPLOGIN_RET)((LPNET_CMD)lpRet)->lpData;
		lpLR->nLength = sizeof(lpLR->lpData);

//		lpLR->lRet = m_pManager->GetResult(MODULE_MSG_LOGIN,
//			((LPNET_CMD)lpData)->lpData,
//			((LPNET_CMD)lpData)->dwLength,
//			lpLR->lpData,&lpLR->nLength);

		((LPNET_CMD)lpRet)->dwLength += (lpLR->nLength - sizeof(lpLR->lpData));
		return 0;
	}

	int TaskPlayback::StopTask() {
		return 0;
	}

	int TaskPlayback::getLoginRes(char*data) {
		LPNET_CMD lpRet = (LPNET_CMD)data;
		lpRet->dwFlag = NET_FLAG;
		lpRet->dwCmd = MODULE_MSG_LOGINRET;
		lpRet->dwLength = sizeof(LOGIN_RET);
		LPLOGIN_RET lpLR = (LPLOGIN_RET)lpRet->lpData;
		lpLR->nLength = sizeof(lpLR->lpData);
		LPFILE_INFO	  fileInfo = (LPFILE_INFO)lpLR->lpData;
		mFfmpeg->getFileInfo(*fileInfo);

		PLAYER_INIT_INFO &playInfo = fileInfo->pi;
		printf("w:%d h:%d size:%d framerate:%d\n",
				playInfo.nWidth, playInfo.nHeigth,
				playInfo.gop_size, playInfo.nFps);

		printf("nAudioFormat:%d nChannel:%d nSampleRate:%d bit_rate:%d\n",
				playInfo.nAudioFormat, playInfo.nChannel,
				playInfo.nSampleRate, playInfo.bit_rate);

		return 0;
	}

	int TaskPlayback::sendEx(char*data,int len) {
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

	void TaskPlayback::packetHead(int fid, short pid, int len, unsigned char type, LPPACK_HEAD lpPack) {
		memset(lpPack, 0, sizeof(PACK_HEAD));
		lpPack->type 		= type;
		lpPack->fid 		= htonl(fid);
		lpPack->pid			= htons(pid);
		lpPack->len 		= htonl(len);
	}

	int TaskPlayback::tpcSendMsg(unsigned char msg) {
		int iRet = 0;
		char sendData[1500] = {0};
		packetHead(0, 0, mPackHeadLen, msg, (LPPACK_HEAD)sendData);
		iRet = sendEx(sendData, mPackHeadLen);
		return iRet;
	}

	int TaskPlayback::tcpSendData(char*data, int len) {
		int iRet = 0;
		char sendData[1500] = {0};
		mSeqid++;
		packetHead(mSeqid, 0, len, VIDEO_RECV_STREAM, (LPPACK_HEAD)sendData);
		iRet = sendEx(sendData, mPackHeadLen);
		iRet = sendEx(data, len);
		return iRet;
	}

	int TaskPlayback::readBuffer() {
		int ret = 0;
		return ret;
	}

	int TaskPlayback::writeBuffer() {
		int ret = 0;
		if(feof(mpFile)) {
			//mRunning = false;
			GLOGW("read file done.");
			return 0;
		}
		int size=GetAnnexbNALU(mpFile, mNALU);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
		//GLOGE("GetAnnexbNALU size:%d", n->len);
		if(size<4)
		{
			GLOGE("get nul error!\n");
			//continue;
		}
		ret = tcpSendData((char*)mNALU->buf, mNALU->len);

		return ret;
	}
