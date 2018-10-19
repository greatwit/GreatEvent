
#include "UdtReceive.h"
#include "recvvideo.h"


	UdtReceive::UdtReceive()
		:mRunning(false)
		//,mSockHandle(0)
	{

	}

	UdtReceive::~UdtReceive() {

	}

	int UdtReceive::create(short localPort) {
		char port[12]={0};
		sprintf(port,"%d",localPort);
		startViedoRecv(port);
		return 0;
	}

	int UdtReceive::release() {
		stopViedoRecv();
		return 0;
	}

	void UdtReceive::registerCallback(IReceiveCallback *base) {
		registerCb(base);
	}

	int UdtReceive::startThread() {
		mRunning = true;

		return 0;
	}

	int UdtReceive::stopThread() {
		return 0;
	}

//
//	int UdtReceive::createServer(UDTSOCKET &serv, char* port) {
//		   // use this function to initialize the UDT library
//		   UDT::startup();
//
//		   addrinfo hints;
//		   addrinfo* res;
//
//		   memset(&hints, 0, sizeof(struct addrinfo));
//		   hints.ai_flags 		= AI_PASSIVE;
//		   hints.ai_family 		= AF_INET;
//		   hints.ai_socktype 	= SOCK_STREAM;//SOCK_STREAM SOCK_DGRAM
//
//		   if (0 != getaddrinfo(NULL, port, &hints, &res))
//		   {
//			   GLOGE("illegal port number or port is busy.\n");
//		       return 0;
//		   }
//
//		   serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
//
//		   if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
//		   {
//			   GLOGE("bind error:%s ", UDT::getlasterror().getErrorMessage());
//		       return 0;
//		   }
//
//		   freeaddrinfo(res);
//
//		   GLOGW("server is ready at port: %s", port);
//
//		   UDT::listen(serv, 10);
//		return 0;
//	}
//
//	int UdtReceive::releaseServer() {
//		   UDT::close(mSendSock);
//		   // use this function to release the UDT library
//		   UDT::cleanup();
//		   GLOGW("releaseServer.");
//		   return 0;
//	}
//
//	int UdtReceive::recvEx(UDTSOCKET usock, char*buf, int size) {
//		int rsize = 0;
//		int rs;
//		while (rsize < size)
//		{
//		  if (UDT::ERROR == (rs = UDT::recv(usock, buf + rsize, size - rsize, 0))){
//
//			  GLOGE("recv error:%s", UDT::getlasterror().getErrorMessage());
//		      return rs;
//		  }
//
//		  rsize += rs;
//		}
//		return rsize;
//	}
//
