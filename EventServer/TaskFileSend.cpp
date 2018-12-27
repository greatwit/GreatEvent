

#include <sys/socket.h>
#include <netinet/in.h>

#include "TaskFileSend.hpp"
#include "BufferCache.hpp"
#include "EventCall.hpp"

#include "event.h"
#include "basedef.h"
#include "protocol.h"
#include "net_protocol.h"

#define	 BUFFER_LEN  1000000  //ref max value 1040000


	TaskFileSend::TaskFileSend( Session*sess, Sid_t& sid, char*filename )
				:mPackHeadLen(sizeof(NET_CMD))
				,TaskBase(sid)
				,mSess(sess)
				,mRecvDataLen(0)
				,mRecvHeadLen(0)
				,mFileLen(0)
				,mHasReadLen(0)
				,mbSendingData(true)
	{
		mRecvBuffer.reset();
		mSendBuffer.reset();

		mInBuffer = new BufferCache();

		mpFile = fopen(filename, "rb");
		fseek( mpFile, 0, SEEK_END );
		mFileLen = ftell( mpFile );
		fseek( mpFile, 0, SEEK_END );
		rewind( mpFile );

		char *lpRet   = mSendBuffer.cmd;
		LPNET_CMD cmd = (LPNET_CMD)lpRet;
		cmd->dwFlag   = NET_FLAG;
		cmd->dwCmd    = MODULE_MSG_LOGINRET;
		cmd->dwIndex  = 0;
		cmd->dwLength = sizeof(LOGIN_RET);

		LOGIN_RET loginRet 	= {0};
		FILE_INFO info 		= {0};
		info.tmEnd 			= mFileLen;
		loginRet.nLength 	= sizeof(FILE_INFO);
		loginRet.lRet 		= ERR_NOERROR;
		memcpy(loginRet.lpData, &info, sizeof(FILE_INFO));
		memcpy(cmd->lpData, &loginRet, sizeof(LOGIN_RET));

		mSendBuffer.totalLen = sizeof(NET_CMD) + sizeof(LOGIN_RET);
		mSendBuffer.bSendCmd = true;
		int ret = tcpSendData();

		//GLOGE("file len:%d totallen:%d\n", mFileLen, mSendBuffer.totalLen);
	}


	TaskFileSend::~TaskFileSend() {
		delete mInBuffer;
		mInBuffer = NULL;

		if(mpFile != NULL)
			fclose(mpFile);
	}


	int TaskFileSend::StartTask() {
		return 0;
	}

	int TaskFileSend::StopTask() {
		return 0;
	}

	//Here further processing is needed.
	int TaskFileSend::writeBuffer() {
		int ret = 0;

		if(mSendBuffer.totalLen==0) //take new data and send,totalLen is cmd len first
		{
			if(mHasReadLen<mFileLen) {
				int iLeftLen = mFileLen - mHasReadLen;
				int iBuffLen = (iLeftLen > BUFFER_LEN)?BUFFER_LEN:iLeftLen;
				mSendBuffer.totalLen = sizeof(NET_CMD) + sizeof(FILE_GET);
				mSendBuffer.bSendCmd = true;
				LPNET_CMD	 cmd = (LPNET_CMD)mSendBuffer.cmd;
				LPFILE_GET frame = (LPFILE_GET)(cmd->lpData);
				cmd->dwFlag 	= NET_FLAG;
				cmd->dwCmd 		= MODULE_MSG_VIDEO;
				cmd->dwIndex 	= 0;
				cmd->dwLength 	= iBuffLen+sizeof(FILE_GET); 	//cmd incidental length
				frame->nLength  = iBuffLen;

				mSendBuffer.data = (char*)malloc(iBuffLen);
				memset(mSendBuffer.data, 0, iBuffLen);
				mSendBuffer.dataLen  = fread(mSendBuffer.data, 1, iBuffLen, mpFile);
				mHasReadLen 		+= mSendBuffer.dataLen;

				GLOGE("fread data len:%d cmd len:%d\n", mSendBuffer.dataLen, cmd->dwLength);
			}
			else{
				ret = pushSendCmd(MODULE_MSG_DATAEND);
				return OWN_SOCK_EXIT;
			}
		}

		ret = tcpSendData();

		return ret;
	}

	int TaskFileSend::sendEx(char*data, int len) {
		int leftLen = len, iRet = 0;

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

	int TaskFileSend::tcpSendData()
	{
		int ret = 0;
		if(mSendBuffer.bSendCmd) {
			ret = sendEx(mSendBuffer.cmd+mSendBuffer.hasSendLen, mSendBuffer.totalLen-mSendBuffer.hasSendLen);
			if(ret>0)
				mSendBuffer.hasSendLen += ret;
			else
				GLOGE("tcpSendData cmd errno:%d ret:%d.", errno, ret);

			if(mSendBuffer.hasSendLen == mSendBuffer.totalLen) {
					mSendBuffer.setToVideo();
			}
		}
		else//data
		{
			ret = sendEx((char*)mSendBuffer.data+mSendBuffer.hasSendLen, mSendBuffer.totalLen-mSendBuffer.hasSendLen);
			if(ret>0)
				mSendBuffer.hasSendLen += ret;
			else
				GLOGE("tcpSendData dta errno:%d ret:%d .\n", errno, ret);

			if(mSendBuffer.hasSendLen == mSendBuffer.totalLen) {
				mSendBuffer.reset();
			}
		}
		return ret;
	}

	int TaskFileSend::pushSendCmd(int iVal, int index) {
		int ret = 0;
		LPNET_CMD	pCmd = (LPNET_CMD)mSendBuffer.cmd;
		switch(iVal) {
			case MODULE_MSG_DATAEND:
			case MODULE_MSG_SEEK_CMPD:
			case MODULE_MSG_SECTION_END:
			case MODULE_MSG_EXERET:
				pCmd->dwFlag 	= NET_FLAG;
				pCmd->dwCmd 	= iVal;
				pCmd->dwIndex 	= index;
				pCmd->dwLength 	= 0;

				mSendBuffer.totalLen = sizeof(NET_CMD);
				mSendBuffer.bSendCmd = true;
				ret = tcpSendData();
				break;
		}
		GLOGE("pushSendCmd value:%d ret:%d.\n", iVal, ret);

		return ret;
	}

	int TaskFileSend::readBuffer() {
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

					GLOGE("playback flag:%08x totalLen:%d ret:%d\n", head->dwFlag, mRecvBuffer.totalLen, ret);
					ret = recvPackData();
				}
			}
		}//
		else{
			ret = recvPackData();
		}
		return ret;
	}

	int TaskFileSend::recvPackData() {
		int &hasRecvLen = mRecvBuffer.hasRecvLen;
		int ret = recv(mSid.mKey, mRecvBuffer.buff+mPackHeadLen+hasRecvLen, mRecvBuffer.totalLen-hasRecvLen, 0);
		//GLOGE("-------------------recvPackData ret:%d\n",ret);
		if(ret>0) {
			hasRecvLen += ret;
			if(hasRecvLen==mRecvBuffer.totalLen) {

				int lValueLen;
			    char acValue[256] = {0};	//new char[256];
			    memset(acValue, 0, 256);
				LPNET_CMD pCmdbuf = (LPNET_CMD)mRecvBuffer.buff;
				if(pCmdbuf->dwCmd == MODULE_MSG_CONTROL_PLAY) {
					PROTO_GetValueByName(mRecvBuffer.buff, (char*)"name", acValue, &lValueLen);
					GLOGE("recv control commond acValue:%s\n",acValue);
					if (strcmp(acValue, "start") == 0) {
						memset(acValue, 0, 256);
						PROTO_GetValueByName(mRecvBuffer.buff, (char*)"tmstart", acValue, &lValueLen);
						GLOGE("tmstart:%d\n",atoi(acValue));

						memset(acValue, 0, 256);
						PROTO_GetValueByName(mRecvBuffer.buff, (char*)"tmend", acValue, &lValueLen);
						GLOGE("tmend:%d\n",atoi(acValue));
						EventCall::addEvent( mSess, EV_WRITE, -1 );
					}
					else if(strcmp(acValue, "setpause") == 0) {
						memset(acValue, 0, 256);
						PROTO_GetValueByName(mRecvBuffer.buff, (char*)"value", acValue, &lValueLen);
						int value = atoi(acValue);
						GLOGE("control setpause value:%d\n", value);
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



