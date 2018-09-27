
#include "TcpSender.h"

#include "spioutils.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

using namespace std;

TcpSender::TcpSender()
		:mInited(false)
		,mFile(NULL)
		,mRunning(false)
		,mSockId(-1)
		,mSid(0)
		,mPackHeadLen(sizeof(PACK_HEAD))
{
	GLOGD("TcpSender::TcpSender construct.");
}

TcpSender::~TcpSender()
{
	// TODO Auto-generated destructor stub
	mInited = false;
}

bool TcpSender::connect(const char* destIp, unsigned short destPort){
	int ret = SP_IOUtils::tcpConnect(destIp, destPort, &mSockId, 0);
	GLOGD("connect ret:%d sockid:%d.",ret, mSockId);
	return true;
}

bool TcpSender::disConnect(){
	if(mSockId>0)
		close(mSockId);
	GLOGE("disConnect");
	return true;
}

bool TcpSender::startFileSend(const char*filename){
	mFile = OpenBitstreamFile(filename);
	GLOGE("filename:%s", filename);
	if (GThread::IsRunning())
		return false;

	if (GThread::Start() < 0)
		return false;
	mRunning = true;
	return true;
}

bool TcpSender::stopFileSend(){
	bool done = false;
	while (GThread::IsRunning() && !done){
		// wait max 5 sec
		usleep(10*1000);
		done = true;
	}

	if (GThread::IsRunning()) {
		GThread::Kill();
	}
	mRunning = false;
	CloseBitstreamFile(mFile);
	return true;
}

void TcpSender::packetHead(int fid, short pid, int len, bool mark, LPPACK_HEAD lpPack) {
	memset(lpPack, 0, sizeof(PACK_HEAD));
	lpPack->type.M 	= (mark==true)?1:0;
	lpPack->fid 		= htonl(fid);
	lpPack->pid			= htons(pid);
	lpPack->len 		= htonl(len);
}

int TcpSender::sendEx(char*data,int len) {
	int leftLen = len, iRet = 0;
	fd_set fdSend;
	struct timeval timeout;
	do {
		FD_ZERO(&fdSend);
		FD_SET(mSockId, &fdSend);
		timeout.tv_sec 	= 1;
		timeout.tv_usec = 0;
		iRet = select(mSockId+1, NULL, &fdSend, NULL, &timeout);
		if(!(iRet>0 && FD_ISSET(mSockId, &fdSend))) {
			if(iRet==0)
				continue;
			return -1;
		}
		if(leftLen<=MAX_MTU)
			iRet = send(mSockId, data+len-leftLen, leftLen, 0);
		else
			iRet = send(mSockId, data+len-leftLen, MAX_MTU, 0);
		leftLen -= iRet;
	}while(leftLen>0);

	return len - leftLen;
}

int TcpSender::tcpSendData(char*data, int len) {
	int leftLen = len,  iRet = 0;
	char sendData[1500] = {0};
	mSid++;
	packetHead(mSid, 0, len, true, (LPPACK_HEAD)sendData);
	iRet = sendEx(sendData, mPackHeadLen);
	iRet = sendEx(data, len);
	return iRet;
}

void *TcpSender::Thread(){
	GThread::ThreadStarted();

	int status = 0;
	NALU_HEADER		*nalu_hdr;
	FU_INDICATOR	*fu_ind;
	FU_HEADER		*fu_hdr;
	//char sendbuf[1500];
	NALU_t *n = AllocNALU(8000000);
	bool start=false;
	while (mRunning){
		if(feof(mFile)) {
			mRunning = false;
			break;
		}
		int size=GetAnnexbNALU(mFile, n);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
		//GLOGE("GetAnnexbNALU size:%d", n->len);
		if(size<4)
		{
			printf("get nul error!\n");
			continue;
		}
		tcpSendData((char*)n->buf, n->len);
		usleep(1*1000);
	}
	return 0;
}
