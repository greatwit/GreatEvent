
#include "TcpSender.h"

#define FILE_NAME "h264/camera_1280x720.h264"

int main(int argc, char **argv) {
	int bRes = 0;
	if(argc<2) {
		printf("usage:app filepath.\n");
		return 1;
	}
	TcpSender *mpTcpSender = new TcpSender();
	mpTcpSender->connect("127.0.0.1", 8080);
	mpTcpSender->startFileSend(argv[1]);
	getchar();
	mpTcpSender->stopFileSend();
	bRes = mpTcpSender->disConnect();
	delete(mpTcpSender);
	return 0;
}
