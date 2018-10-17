
#ifndef WIN32
   #include <arpa/inet.h>
   #include <netdb.h>
#else
   #include <winsock2.h>
   #include <ws2tcpip.h>
#endif


#include "sendvideo.h"
#include "basedef.h"
#include "h264.h"

UDTSOCKET mRecvSock;
fstream	  mrecvStream;


int createConnect(UDTSOCKET &sockHandle, char*ipaddr, char*port) {
	   // use this function to initialize the UDT library
	   UDT::startup();

	   struct addrinfo hints, *peer;

	   memset(&hints, 0, sizeof(struct addrinfo));
	   hints.ai_flags 		= AI_PASSIVE;
	   hints.ai_family 		= AF_INET;
	   hints.ai_socktype 	= SOCK_STREAM;//SOCK_STREAM SOCK_DGRAM

	   sockHandle = UDT::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

	   if (0 != getaddrinfo(ipaddr, port, &hints, &peer))
	   {
		   GLOGE("incorrect server/peer address ip:%s port:%s. ", ipaddr, port);
	      return -1;
	   }

	   // connect to the server, implict bind
	   if (UDT::ERROR == UDT::connect(sockHandle, peer->ai_addr, peer->ai_addrlen))
	   {
		   GLOGE("connect error:%s ", UDT::getlasterror().getErrorMessage());
	      return -1;
	   }

	   freeaddrinfo(peer);

	   return 0;
}

int releaseConnect(UDTSOCKET fhandle) {
	UDT::close(fhandle);
    // use this function to release the UDT library
    UDT::cleanup();
    GLOGW("releaseConnect.");
	return 0;
}

int stopRecv() {
	mrecvStream.close();
	GLOGW("stopRecv.");
	return 0;
}

int sendEx(UDTSOCKET usock, char*buf, int size) {
	   int ssize = 0;
	   int ss;
	   while (ssize < size) {
	     if (UDT::ERROR == (ss = UDT::send(usock, buf + ssize, size - ssize, 0))) {
	       GLOGE("send error:%s", UDT::getlasterror().getErrorMessage());
	       return ss;
	     }
	     ssize += ss;
	   }
	   return ssize;
}

void* sendfile(void* usocket)
{
   UDTSOCKET fhandle = *(UDTSOCKET*)usocket;
   delete (UDTSOCKET*)usocket;

	FILE			*mwFile = OpenBitstreamFile( "camera_640x480.h264" );
	NALU_t 			*mNALU  = AllocNALU(8000000);
	int ret = 0;
	UDT::TRACEINFO trace;
	UDT::perfmon(fhandle, &trace);
	while(true) {
		if(feof(mwFile)) {
			//mRunning = false;
			GLOGW("read file done.");
			break;
		}
		int size=GetAnnexbNALU(mwFile, mNALU);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
		//GLOGE("GetAnnexbNALU size:%d", n->len);
		if(size<4){
			GLOGE("get nul error!\n");
			break;
		}

		// send file size information
		if (UDT::ERROR == sendEx(fhandle, (char*)&mNALU->len, sizeof(int) )) {
			GLOGE("send error:%s ", UDT::getlasterror().getErrorMessage());
		    return 0;
		}

		ret = sendEx(fhandle, (char*)mNALU->buf, mNALU->len);
		GLOGE("ret:%d len:%d ", ret, mNALU->len);
	}

	UDT::perfmon(fhandle, &trace);
	GLOGE("speed = %f Mbits/sec ", trace.mbpsSendRate);

	UDT::close(fhandle);

    if(mwFile != NULL)
		fclose(mwFile);

	FreeNALU(mNALU);

    return NULL;
}

int startVideoSend(char*ipaddr, char*port, char*needFile, char*saveFile) {
	createConnect(mRecvSock, ipaddr, port);

    pthread_t recvthread;
    pthread_create(&recvthread, NULL, sendfile, new UDTSOCKET(mRecvSock));
    pthread_detach(recvthread);

	//startRecv(mRecvSock, needFile, saveFile);

	return 0;
}

int stopVideoSend() {

	stopRecv();
	releaseConnect(mRecvSock);
	return 0;
}

#ifndef __ANDROID__
int main(int argc, char* argv[])
{
   if ((argc != 5) || (0 == atoi(argv[2])))
   {
      cout << "usage: sendvideo server_ip server_port remote_filename local_filename" << endl;
      return -1;
   }

   startVideoSend(argv[1],argv[2],argv[3],argv[4]);

   getchar();

   stopVideoSend();

   return 0;
}
#endif

