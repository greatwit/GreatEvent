

#include <sys/socket.h>
#include <netinet/in.h>

#include "TaskPlayback.hpp"
#include "BufferCache.hpp"
#include "EventCall.hpp"

#include "event.h"
#include "basedef.h"
#include "protocol.h"
#include "net_protocol.h"


#define	FILE_PATH	"h264/camera_640x480.h264"

	TaskPlayback::TaskPlayback( Session* sess, Sid_t& sid, char*filename )
				:mPackHeadLen(sizeof(NET_CMD))
				,TaskBase(sid)
				,mSess(sess)
				,mFfmpeg(NULL)
				,mpFile(NULL)
				,mSeqid(0)
	{
		//GLOGD("TaskPlayback filename:%s.", FILE_PATH);
		GLOGE("mPackHeadLen is :%d", mPackHeadLen);
		mRecvBuffer.reset();

		mSendBuffer.reset();
		mInBuffer = new BufferCache();

		mFfmpeg   = new FfmpegContext(filename);

		//char lpRet[sizeof(NET_CMD) + sizeof(LOGIN_RET)] = {0};
		char *lpRet = mSendBuffer.cmd;
		((LPNET_CMD)lpRet)->dwFlag = NET_FLAG;
		((LPNET_CMD)lpRet)->dwCmd  = MODULE_MSG_LOGINRET;
		((LPNET_CMD)lpRet)->dwLength = sizeof(LOGIN_RET);
		LOGIN_RET loginRet = {0};
		getLoginRet(loginRet);
		loginRet.lRet = ERR_NOERROR;
		memcpy(lpRet+sizeof(NET_CMD), &loginRet, sizeof(LOGIN_RET));
		int ret = sendEx(lpRet, sizeof(NET_CMD) + sizeof(LOGIN_RET));

//		mpFile = OpenBitstreamFile( filename ); //camera_640x480.h264 //camera_1280x720.h264
//		if(mpFile==NULL) {
//			GLOGE("open file:%s failed.", filename);
//		}
		//mNALU  = AllocNALU(8000000);
//
//		tpcSendMsg(VIDEO_RECV_MSG);

		GLOGE("net_cmd len:%d send ret:%d", mPackHeadLen, ret);
		//StartTask();
		EventCall::addEvent( mSess, EV_WRITE, -1 );
	}


	TaskPlayback::~TaskPlayback() {
		delete mInBuffer;
		mInBuffer = NULL;

		if(mpFile != NULL)
			fclose(mpFile);

		//FreeNALU(mNALU);

		if(mFfmpeg!=NULL) {
			delete mFfmpeg;
			mFfmpeg = NULL;
		}
	}

	int TaskPlayback::StartTask() {
		char lpRet[sizeof(NET_CMD) + sizeof(AV_FRAME)];
		ZeroMemory(lpRet, sizeof(lpRet));
		LPNET_CMD	 cmd   = (LPNET_CMD)lpRet;
		LPAV_FRAME frame = (LPAV_FRAME)(lpRet+sizeof(NET_CMD));
		AVPacket pkt;
		int res = mFfmpeg->getPackageData(pkt);
		if(res>=0) {
			frame->dwFrameType 	= pkt.flags+1;
			frame->dwTick 		= pkt.pts;
			frame->dwTm   		= pkt.pts;
			frame->nLength 		= pkt.size;//frame size

			cmd->dwFlag 		= NET_FLAG;
			cmd->dwCmd 		= MODULE_MSG_VIDEO;
			cmd->dwIndex 	= 0;
			cmd->dwLength 	= pkt.size+sizeof(AV_FRAME);
		}
		GLOGE("video cmd len:%d", sizeof(NET_CMD) + sizeof(AV_FRAME));
		return 0;
	}

	int TaskPlayback::StopTask() {
		return 0;
	}

	int TaskPlayback::getLoginRet(LOGIN_RET& lRet) {
//		LPNET_CMD lpRet = (LPNET_CMD)(*data);
//		lpRet->dwFlag = NET_FLAG;
//		lpRet->dwCmd = MODULE_MSG_LOGINRET;
//		lpRet->dwLength = sizeof(LOGIN_RET);
//		LPLOGIN_RET lpLR = (LPLOGIN_RET)lpRet->lpData;
//		lpLR->nLength = sizeof(lpLR->lpData);

		LPFILE_INFO	  fileInfo = (LPFILE_INFO)lRet.lpData;
		lRet.nLength = sizeof(FILE_INFO);
		mFfmpeg->getFileInfo(*fileInfo);

		PLAYER_INIT_INFO &playInfo = fileInfo->pi;
		printf("getLoginRet w:%d h:%d size:%d framerate:%d\n",
				playInfo.nWidth, playInfo.nHeigth,
				playInfo.gop_size, playInfo.nFps);

		printf("getLoginRet nAudioFormat:%d nChannel:%d nSampleRate:%d bit_rate:%d\n",
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
			if(leftLen <= MAX_MTU)
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
		int ret = -1;
		int &hasRecvLen = mRecvBuffer.hasRecvLen;
		if(mRecvBuffer.bRecvHead) {
			ret = recv(mSid.mKey, mRecvBuffer.buff+hasRecvLen, mPackHeadLen-hasRecvLen, 0);
			if(ret>0) {
				hasRecvLen+=ret;
				if(hasRecvLen==mPackHeadLen) {
					LPNET_CMD head = (LPNET_CMD)mRecvBuffer.buff;
					mRecvBuffer.totalLen = head->dwLength;
					mRecvBuffer.bRecvHead = false;
					hasRecvLen = 0;

					GLOGE("playback flag:%08x totalLen:%d ret:%d", head->dwFlag, mRecvBuffer.totalLen, ret);
					//GLOGE("Session flag:%08x ret:%d data:%s", cmdbuf->dwFlag, ret, cmdbuf->lpData);
					ret = recvPackData();
				}
			}
		}//
		else{
			ret = recvPackData();
		}
		return ret;
	}

	int TaskPlayback::recvPackData() {
		int &hasRecvLen = mRecvBuffer.hasRecvLen;
		int ret = recv(mSid.mKey, mRecvBuffer.buff+mPackHeadLen+hasRecvLen, mRecvBuffer.totalLen-hasRecvLen, 0);
		GLOGE("recvPackData ret:%d",ret);
		if(ret>0) {
			hasRecvLen += ret;
			if(hasRecvLen==mRecvBuffer.totalLen) {

				int lValueLen;
			    char acValue[256] = {0};//new char[256];
			    memset(acValue,0, 256);
				LPNET_CMD pCmdbuf = (LPNET_CMD)mRecvBuffer.buff;
			    PROTO_GetValueByName(mRecvBuffer.buff, "name", acValue, &lValueLen);
			    GLOGE("name:%s",acValue);

			    memset(acValue,0, 256);
			    PROTO_GetValueByName(mRecvBuffer.buff, "tmstart", acValue, &lValueLen);
			    GLOGE("tmstart:%d",atoi(acValue));

			    memset(acValue,0, 256);
			    PROTO_GetValueByName(mRecvBuffer.buff, "tmend", acValue, &lValueLen);
			    GLOGE("tmend:%d",atoi(acValue));
			    //GLOGE("recv total:%s", mRecvBuffer.buff);

			    mRecvBuffer.reset();
			}
		}
		return ret;
	}

	int TaskPlayback::writeBuffer() {
		int ret = 0;
//		if(feof(mpFile)) {
//			//mRunning = false;
//			GLOGW("read file done.");
//			return 0;
//		}
//		int size=GetAnnexbNALU(mpFile, mNALU);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
//		//GLOGE("GetAnnexbNALU size:%d", n->len);
//		if(size<4)
//		{
//			GLOGE("get nul error!\n");
//			//continue;
//		}
//		ret = tcpSendData((char*)mNALU->buf, mNALU->len);
		//GLOGE("TaskPlayback writeBuffer event.");

		return ret;
	}
