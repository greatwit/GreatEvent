#include "UdtSender.h"

#include "sendvideo.h"

	UdtSender::UdtSender() {

	}

	UdtSender::~UdtSender() {

	}

	int	UdtSender::create(short localPort) {
		return 0;
	}

    int	UdtSender::release() {
    	return 0;
    }

    int UdtSender::connect(char* sDestIp, short destPort) {
		char port[12]={0};
		sprintf(port,"%d",destPort);
    	return startVideoSend(sDestIp, port);
    }

	int UdtSender::disConnect() {
		return stopVideoSend();
	}

	int UdtSender::sendBuffer(void*buff, int dataLen, int64_t timeStamp) {
		return sendData((char*)buff, dataLen);
	}
