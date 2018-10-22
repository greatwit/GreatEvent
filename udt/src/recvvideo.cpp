

#ifndef WIN32
   #include <cstdlib>
   #include <netdb.h>
#else
   #include <winsock2.h>
   #include <ws2tcpip.h>
#endif

#ifndef WIN32
void* sendfile(void*);
#else
DWORD WINAPI sendfile(LPVOID);
#endif


#include "recvvideo.h"
#include "basedef.h"


UDTSOCKET mSendSock = 0;
IReceiveCallback	*mStreamBase = NULL;
static bool mRunning = false;


int createServer(UDTSOCKET &serv, char* port) {
	   // use this function to initialize the UDT library
	   UDT::startup();

	   addrinfo hints;
	   addrinfo* res;

	   memset(&hints, 0, sizeof(struct addrinfo));
	   hints.ai_flags 		= AI_PASSIVE;
	   hints.ai_family 		= AF_INET;
	   hints.ai_socktype 	= SOCK_STREAM;//SOCK_STREAM SOCK_DGRAM

	   if (0 != getaddrinfo(NULL, port, &hints, &res))
	   {
		   GLOGE("illegal port number or port is busy.\n");
	       return 0;
	   }

	   serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	   if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
	   {
		   GLOGE("bind error:%s ", UDT::getlasterror().getErrorMessage());
	       return 0;
	   }

	   freeaddrinfo(res);

	   GLOGW("server is ready at port: %s", port);

	   UDT::listen(serv, 10);

	return 0;
}

int releaseServer() {
	if(mSendSock>0) {
	   UDT::close(mSendSock);
	   mSendSock = 0;
	}
	   // use this function to release the UDT library
	   UDT::cleanup();
	   GLOGW("releaseServer.");
	   return 0;
}

pthread_t start(void *(*func_routine)(void*)) {
	mRunning = true;

	pthread_t acceptthread;
	pthread_create(&acceptthread, NULL, func_routine, &mSendSock);
	pthread_detach(acceptthread);
	return acceptthread;
}

int stop() {
	return 0;
}

int recvEx(UDTSOCKET usock, char*buf, int size) {
	int rsize = 0;
	int rs;
	while (rsize < size)
	{
	  if (UDT::ERROR == (rs = UDT::recv(usock, buf + rsize, size - rsize, 0))){

	    cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
	    return rs;
	  }

	  rsize += rs;
	}
	return rsize;
}

void* recvvideo(void* usocket)
{
	   UDTSOCKET fhandle = *(UDTSOCKET*)usocket;
	   delete (UDTSOCKET*)usocket;

	   int size=0, ret = 0;
	   // aquiring file name information from client
	   char buffer[800000] = {0};
	   while(mRunning) {
		   if (UDT::ERROR == recvEx(fhandle, (char*)&size, sizeof(int))) {
			   GLOGE("recv len error:%s", UDT::getlasterror().getErrorMessage());
			   return 0;
		   }

		   if (UDT::ERROR == (ret = recvEx(fhandle, buffer, size))) {
			   GLOGE("recv data len:%s", UDT::getlasterror().getErrorMessage());
		   }
		   if(mStreamBase)
			   mStreamBase->ReceiveSource(0, "", (void *)buffer, size);

		   //GLOGW("recv data ret:%d rsize:%d", ret, size);
	  }

	  return 0;
}

void* acceptEvent(void* addr) {
	UDTSOCKET serv = *(UDTSOCKET*)addr;
	   while (true)
	   {
		  UDTSOCKET fhandle;
		  sockaddr_storage clientaddr;
		  int addrlen = sizeof(clientaddr);
	      if (UDT::INVALID_SOCK == (fhandle = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
	      {
	    	  GLOGE("accept error:%s ", UDT::getlasterror().getErrorMessage());
	          return 0;
	      }

	      char clienthost[NI_MAXHOST];
	      char clientservice[NI_MAXSERV];
	      getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
	      GLOGW("new connection clienthost:%s clientservice:%s", clienthost, clientservice);

	      #ifndef WIN32
	         pthread_t filethread;
	         pthread_create(&filethread, NULL, recvvideo, new UDTSOCKET(fhandle));
	         pthread_detach(filethread);
	      #else
	         CreateThread(NULL, 0, sendfile, new UDTSOCKET(fhandle), 0, NULL);
	      #endif
	   }
}

int startViedoRecv(char*port) {
	createServer(mSendSock, port);
	start(acceptEvent);
	return 0;
}

int stopViedoRecv() {
	mRunning = false;

    stop();
    releaseServer();
	return 0;
}

void registerCb(IReceiveCallback *base) {
	mStreamBase = base;
}

//#ifndef __ANDROID__
//int main(int argc, char* argv[])
//{
//	startViedoRecv("9000");
//
//    getchar();
//
//    stopViedoRecv();
//
//    return 0;
//}
//#endif

