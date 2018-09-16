
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

void TcpSender::packetHead(short len, int &id, bool mark, LPPACK_HEAD lpPack) {
	memset(lpPack, 0, sizeof(PACK_HEAD));
	lpPack->type.M 	= (mark==true)?1:0;
	lpPack->id 		= htonl(id);
	lpPack->len 	= htons(len);
	id++;
}

int TcpSender::tcpSendData(char*data, int len){
	int sendLen = len, iRet = 0;
	char sendData[1500] = {0};
	if(len <= MAX_LEN) {
		packetHead(len+8, mSid, true, (LPPACK_HEAD)sendData);
		memcpy(sendData+8, data, len);
		iRet += send(mSockId, sendData, len+8, 0);
	}
	else {
		bool bFirstPack = true;
		while(sendLen>0){
			if(sendLen<=MAX_LEN) {
					packetHead(sendLen+8, mSid, false, (LPPACK_HEAD)sendData);
					memcpy(sendData+8, data+len-sendLen, sendLen);
		     		iRet += send(mSockId, sendData, sendLen+8,0);
			}
			else {
				packetHead(MAX_MTU, mSid, bFirstPack, (LPPACK_HEAD)sendData);
				memcpy(sendData+8, data+len-sendLen, MAX_LEN);
				iRet += send(mSockId, sendData, MAX_MTU,0);
				bFirstPack = false;
			}
			sendLen -= MAX_LEN;
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
		GLOGE("GetAnnexbNALU size:%d", size);
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
