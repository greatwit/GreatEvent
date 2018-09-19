
#include "TcpSender.h"

#include "spioutils.h"
#include <unistd.h>
#include <sys/socket.h>
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

int TcpSender::tcpSendData(char*data, int len){
	int leftLen = len, sendLen = len + mPackHeadLen, iRet = 0, pid = 0;
	char sendData[1500] = {0};
	mSid++;

	if(len <= MAX_LEN) {
		pid++;
		packetHead( mSid, pid, sendLen, true, (LPPACK_HEAD)sendData);
		memcpy(sendData+mPackHeadLen, data, len);
		iRet += send(mSockId, sendData, len+mPackHeadLen, 0);
		printf("fid:%d pid:%d len:%d\n", mSid, pid, len);
	}
	else {
		bool bFirstPack = true;
		while(leftLen>0){
			pid++;
			if(leftLen<=MAX_LEN) {
					packetHead(mSid, pid, sendLen, false, (LPPACK_HEAD)sendData);
					memcpy(sendData+mPackHeadLen, data+len-leftLen, leftLen);
		     		iRet += send(mSockId, sendData, leftLen+mPackHeadLen,0);
		     		printf("fid:%d pid:%d len:%d\n", mSid, pid, len);
			}
			else {
				packetHead(mSid, pid, sendLen, bFirstPack, (LPPACK_HEAD)sendData);
				memcpy(sendData+mPackHeadLen, data+len-leftLen, MAX_LEN);
				iRet += send(mSockId, sendData, MAX_MTU,0);
				bFirstPack = false;
				printf("fid:%d pid:%d len:%d\n", mSid, pid, len);
			}
			leftLen -= MAX_LEN;
		}
	}
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
		GLOGE("GetAnnexbNALU size:%d", n->len);
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
