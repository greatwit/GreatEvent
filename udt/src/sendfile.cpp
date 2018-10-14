

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


#include "sendfile.h"
#include "basedef.h"


UDTSOCKET mSendSock;

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
	   UDT::close(mSendSock);
	   // use this function to release the UDT library
	   UDT::cleanup();
	   GLOGW("releaseServer.");
	   return 0;
}

pthread_t start(void *(*func_routine)(void*)) {
	pthread_t acceptthread;
	pthread_create(&acceptthread, NULL, func_routine, &mSendSock);
	pthread_detach(acceptthread);
	return acceptthread;
}

int stop() {
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
	         pthread_create(&filethread, NULL, sendfile, new UDTSOCKET(fhandle));
	         pthread_detach(filethread);
	      #else
	         CreateThread(NULL, 0, sendfile, new UDTSOCKET(fhandle), 0, NULL);
	      #endif
	   }
}


#ifndef WIN32
void* sendfile(void* usocket)
#else
DWORD WINAPI sendfile(LPVOID usocket)
#endif
{
   UDTSOCKET fhandle = *(UDTSOCKET*)usocket;
   delete (UDTSOCKET*)usocket;

   // aquiring file name information from client
   char file[1024];
   int len;

   if (UDT::ERROR == UDT::recv(fhandle, (char*)&len, sizeof(int), 0))
   {
	  GLOGE("recv error:%s ", UDT::getlasterror().getErrorMessage());
      return 0;
   }

   if (UDT::ERROR == UDT::recv(fhandle, file, len, 0))
   {
	  GLOGE("recv error:%s ", UDT::getlasterror().getErrorMessage());
      return 0;
   }
   file[len] = '\0';
   GLOGW("file len:%d filename:%s", len, file);

   // open the file
   fstream ifs(file, ios::in | ios::binary);

   ifs.seekg(0, ios::end);
   int64_t size = ifs.tellg();
   ifs.seekg(0, ios::beg);

   // send file size information
   if (UDT::ERROR == UDT::send(fhandle, (char*)&size, sizeof(int64_t), 0))
   {
	   GLOGE("send error:%s ", UDT::getlasterror().getErrorMessage());
      return 0;
   }

   UDT::TRACEINFO trace;
   UDT::perfmon(fhandle, &trace);

   // send the file
   int64_t offset = 0;
   if (UDT::ERROR == UDT::sendfile(fhandle, ifs, offset, size))
   {
	   GLOGE("sendfile error:%s ", UDT::getlasterror().getErrorMessage());
      return 0;
   }

   UDT::perfmon(fhandle, &trace);
   GLOGE("speed = %f Mbits/sec", trace.mbpsSendRate);

   UDT::close(fhandle);

   ifs.close();

   #ifndef WIN32
      return NULL;
   #else
      return 0;
   #endif
}

int startFileSend(char*port) {
	createServer(mSendSock, port);
	start(acceptEvent);

	GLOGW("startFileSend.");
	return 0;
}

int stopFileSend() {
    stop();
    releaseServer();

    GLOGW("stopFileSend.");
	return 0;
}


#ifndef __ANDROID__
int main(int argc, char* argv[])
{
	startFileSend("9000");

    getchar();

    stopFileSend();

    return 0;
}
#endif

