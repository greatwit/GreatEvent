

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
				,mFrameRate(30)
				,mFrameCount(0)
	{
		//GLOGD("TaskPlayback filename:%s.", FILE_PATH);
		GLOGE("mPackHeadLen is :%d", mPackHeadLen);
		mRecvBuffer.reset();
		mSendBuffer.reset();

		mInBuffer = new BufferCache();
		mFfmpeg   = new FfmpegContext(filename);

		//char lpRet[sizeof(NET_CMD) + sizeof(LOGIN_RET)] = {0};
		char *lpRet   = mSendBuffer.cmd;
		LPNET_CMD cmd = (LPNET_CMD)lpRet;
		cmd->dwFlag   = NET_FLAG;
		cmd->dwCmd    = MODULE_MSG_LOGINRET;
		cmd->dwIndex  = 0;
		cmd->dwLength = sizeof(LOGIN_RET);
		LOGIN_RET loginRet = {0};
		getLoginRet(loginRet);
		loginRet.lRet = ERR_NOERROR;
		memcpy(cmd->lpData, &loginRet, sizeof(LOGIN_RET));

		mSendBuffer.totalLen = sizeof(NET_CMD) + sizeof(LOGIN_RET);
		mSendBuffer.bSendCmd = true;
		int ret = tcpSendData();

		//GLOGE("net_cmd len:%d send ret:%d", mPackHeadLen, mMsgQueue.getSize());
		//StartTask();
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


		GLOGE("queue size:%d", mMsgQueue.getSize());
		mMsgQueue.clearQueue();
//		int size = mMsgQueue.getSize();
//		for(int i=0;i<size;i++) {
//			int val = 0;
//			mMsgQueue.try_pop(val);
//			GLOGE("queue value:%d", val);
//		}


	}

	int TaskPlayback::StartTask() {

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

		//LPFILE_INFO	  fileInfo = (LPFILE_INFO)lRet.lpData;

		FILE_INFO info = {0};
		lRet.nLength = sizeof(FILE_INFO);
		mFfmpeg->getFileInfo(info);

		PLAYER_INIT_INFO &playInfo = info.pi;
		memcpy(lRet.lpData, &info, sizeof(FILE_INFO));

		mFrameRate = playInfo.nFps;

		GLOGE("getLoginRet w:%d h:%d size:%d framerate:%d extlen:%d\n",
				playInfo.nWidth, playInfo.nHeigth,
				playInfo.gop_size, playInfo.nFps, playInfo.extsize);

		GLOGE("getLoginRet nAudioFormat:%d nChannel:%d nSampleRate:%d bit_rate:%d\n",
				playInfo.nAudioFormat, playInfo.nChannel,
				playInfo.nSampleRate, playInfo.bit_rate);

		return 0;
	}

	int TaskPlayback::sendEx(char*data, int len) {
		int leftLen = len, iRet = 0;
		fd_set fdSend;
		struct timeval timeout;
		int sockId = mSid.mKey;
		do {
			if(leftLen <= MAX_MTU)
				iRet = send(sockId, data+len-leftLen, leftLen, 0);
			else
				iRet = send(sockId, data+len-leftLen, MAX_MTU, 0);
			if(iRet<0) {
				//GLOGE("send data errno:%d ret:%d.", errno, iRet);
				return iRet;
			}
			leftLen -= iRet;
		}while(leftLen>0);

		return len - leftLen;
	}

//	void TaskPlayback::packetHead(int fid, short pid, int len, unsigned char type, LPPACK_HEAD lpPack) {
//		memset(lpPack, 0, sizeof(PACK_HEAD));
//		lpPack->type 		= type;
//		lpPack->fid 		= htonl(fid);
//		lpPack->pid			= htons(pid);
//		lpPack->len 		= htonl(len);
//	}
//
//	int TaskPlayback::tpcSendMsg(unsigned char msg) {
//		int iRet = 0;
//		char sendData[1500] = {0};
//		packetHead(0, 0, mPackHeadLen, msg, (LPPACK_HEAD)sendData);
//		iRet = sendEx(sendData, mPackHeadLen);
//		return iRet;
//	}

	int TaskPlayback::setHeartCount() {
		if(mMsgQueue.getSize() < 10)
			mMsgQueue.push(MODULE_MSG_PING);
		return ++mHeartCount;
	}

	int TaskPlayback::tcpSendData()
	{
		int ret = 0;
		if(mSendBuffer.bSendCmd) {
			ret = sendEx(mSendBuffer.cmd+mSendBuffer.hasSendLen, mSendBuffer.totalLen-mSendBuffer.hasSendLen);
			if(ret>0)
				mSendBuffer.hasSendLen += ret;
			else
				GLOGE("tcpSendData cmd errno:%d ret:%d.", errno, ret);
			if(mSendBuffer.hasSendLen == mSendBuffer.totalLen)
			{
				if(mSendBuffer.avpack.size>0)
					mSendBuffer.setToVideo();
				else
					mSendBuffer.reset();
			}
		}
		else
		{
			ret = sendEx((char*)mSendBuffer.avpack.data+mSendBuffer.hasSendLen, mSendBuffer.totalLen-mSendBuffer.hasSendLen);
			if(ret>0)
				mSendBuffer.hasSendLen += ret;
			else
				GLOGE("tcpSendData dta errno:%d ret:%d.", errno, ret);

			if(mSendBuffer.hasSendLen == mSendBuffer.totalLen) {
				mSendBuffer.reset();
			}
		}
		return ret;
	}

	int TaskPlayback::sendEndfileCmd() {
		LPNET_CMD	cmd = (LPNET_CMD)mSendBuffer.cmd;
		cmd->dwFlag 	= NET_FLAG;
		cmd->dwCmd 		= MODULE_MSG_DATAEND;
		cmd->dwIndex 	= 0;
		cmd->dwLength 	= 0;

		mSendBuffer.totalLen = sizeof(NET_CMD);
		mSendBuffer.bSendCmd = true;
		int ret = tcpSendData();

		return ret;
	}

	int TaskPlayback::sendHearbeatCmd() {
		LPNET_CMD	cmd = (LPNET_CMD)mSendBuffer.cmd;
		cmd->dwFlag 	= NET_FLAG;
		cmd->dwCmd 		= MODULE_MSG_PING;
		cmd->dwIndex 	= 0;
		cmd->dwLength 	= 0;
		mSendBuffer.totalLen = sizeof(NET_CMD);
		mSendBuffer.bSendCmd = true;
		int ret = tcpSendData();
	}

	int TaskPlayback::writeBuffer() {
		static int count = 0;
		int ret = 0;

		if(mSendBuffer.totalLen==0) //take new data and send
		{
			int frameType = -1;
			AVPacket &pkt 	 = mSendBuffer.avpack;
			int res = mFfmpeg->getPackageData(pkt, frameType);
			if(res>=0) {
				mSendBuffer.totalLen = sizeof(NET_CMD) + sizeof(AV_FRAME);
				mSendBuffer.bSendCmd = true;

				LPNET_CMD	 cmd = (LPNET_CMD)mSendBuffer.cmd;
				LPAV_FRAME frame = (LPAV_FRAME)(cmd->lpData);

				cmd->dwFlag 	= NET_FLAG;
				cmd->dwCmd 		= MODULE_MSG_VIDEO;
				cmd->dwIndex 	= 0;
				cmd->dwLength 	= pkt.size+sizeof(AV_FRAME);
				frame->nLength 		= pkt.size;//frame size
				frame->dwTick 		= pkt.pts;
				frame->dwTm   		= pkt.pts;

				switch(frameType) {
					case AVMEDIA_TYPE_VIDEO:
						frame->dwFrameType = (pkt.flags == 0)?FRAME_VIDEO_P:FRAME_VIDEO_I;
						if(frame->dwFrameType==FRAME_VIDEO_I && pkt.data[4]==0x67) {
							int index = 0,len = 0;
							char head[4] = {0,0,0,1};
							for(int i=0;i<36;i++) {
								char curDat = pkt.data[i];
								if(curDat==0x67 || curDat==0x68) {
									index 	= i;
									len 	= (int)pkt.data[i-1];
									memcpy(pkt.data+i-4, head, 4);
									printf("---------------------index:%d len:%d\n", index, len);
								}
							}
							memcpy(pkt.data+index+len, head, 4);
						}
						if(pkt.size>=4) {
							pkt.data[0] = 0x00;
							pkt.data[1] = 0x00;
							pkt.data[2] = 0x00;
							pkt.data[3] = 0x01;
						}
						break;

					case AVMEDIA_TYPE_AUDIO:
						frame->dwFrameType = FRAME_AUDIO;
						break;
				}

//				if(mFrameCount>=0) {
//					mFrameCount++;
//					if(mFrameCount>=300) {
//						mFrameCount = -1;
//						return 0;
//					}
//				}

			}
			else {
				ret = sendEndfileCmd();
				GLOGE("TaskPlayback sendEndfileCmd ret:%d.", ret);
				return 0;
			}
		}

		//still have data
		ret = tcpSendData();

		count++;
		//if(count >= 2) return -1;
		GLOGE("TaskPlayback writeBuffer len:%d ret:%d count:%d.", sizeof(NET_CMD) + sizeof(AV_FRAME), ret, count);
		//usleep(10*1000);

		return ret;
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
					mRecvBuffer.totalLen  = head->dwLength;
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
		GLOGE("-------------------recvPackData ret:%d",ret);
		if(ret>0) {
			hasRecvLen += ret;
			if(hasRecvLen==mRecvBuffer.totalLen) {

				int lValueLen;
			    char acValue[256] = {0};//new char[256];
			    memset(acValue,0, 256);
				LPNET_CMD pCmdbuf = (LPNET_CMD)mRecvBuffer.buff;
				if(pCmdbuf->dwCmd == MODULE_MSG_CONTROL_PLAY) {
					PROTO_GetValueByName(mRecvBuffer.buff, (char*)"name", acValue, &lValueLen);
					if (strcmp(acValue, "start") == 0) {
						memset(acValue,0, 256);
						PROTO_GetValueByName(mRecvBuffer.buff, (char*)"tmstart", acValue, &lValueLen);
						GLOGE("tmstart:%d",atoi(acValue));

						memset(acValue,0, 256);
						PROTO_GetValueByName(mRecvBuffer.buff, (char*)"tmend", acValue, &lValueLen);
						GLOGE("tmend:%d",atoi(acValue));
						EventCall::addEvent( mSess, EV_WRITE, -1 );
					}
					else if(strcmp(acValue, "setpause") == 0) {
						memset(acValue,0, 256);
						PROTO_GetValueByName(mRecvBuffer.buff, (char*)"value", acValue, &lValueLen);
					}
					else if(strcmp(acValue, "seek") == 0) {
						memset(acValue,0, 256);
						PROTO_GetValueByName(mRecvBuffer.buff, (char*)"value", acValue, &lValueLen);

						char *lpRet   = mSendBuffer.cmd;
						LPNET_CMD cmd = (LPNET_CMD)lpRet;
						cmd->dwFlag   = NET_FLAG;
						cmd->dwCmd    = MODULE_MSG_SEEK_CMPD;
						cmd->dwIndex  = 0;
						cmd->dwLength = sizeof(LOGIN_RET);
					}
				}
			    //GLOGE("recv total:%s", mRecvBuffer.buff);

			    mRecvBuffer.reset();
			}
		}
		else if(ret == 0) {

		}

		return ret;
	}

