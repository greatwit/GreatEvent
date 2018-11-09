#ifndef TcpSender_H_
#define TcpSender_H_

#include "h264.h"
#include "gthread.h"

#include <iostream>


#include "basedef.h"

#define TAG "H264Sender"


class TcpSender : private GThread
{
	public:
	TcpSender();
		virtual ~TcpSender();

		bool connect(const char* destIp, unsigned short destPort);
		bool disConnect();

		bool startFileSend(const char*filename);
		bool stopFileSend();

		void *Thread();

	protected:
		void packetHead(int fid, short pid, int len, unsigned char type, LPPACK_HEAD lpPack);
		int tcpSendData(char*data, int len);
		int tpcSendMsg(unsigned char msg);
		int sendEx(char*data,int len);

	private:
		int 			mSockId;
		int				mSeqid;
		int 			mPackHeadLen;
		FILE			*mFile;
		bool 			mRunning;
		bool			mInited;
};

#endif
