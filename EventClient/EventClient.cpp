
#include "TcpSender.h"
#define FILE_NAME "camera_640x480.h264"
int main(int argc, char **argv) {
	int bRes = 0;
	TcpSender *mpTcpSender = new TcpSender();
	mpTcpSender->connect("127.0.0.1", 8080);
	mpTcpSender->startFileSend(FILE_NAME);
	getchar();
	mpTcpSender->stopFileSend();
	bRes = mpTcpSender->disConnect();
	delete(mpTcpSender);
	return 0;
}
