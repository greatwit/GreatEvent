

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


UDTSOCKET mSendSock;

int createServer(UDTSOCKET &serv, string port) {
	   // use this function to initialize the UDT library
	   UDT::startup();

	   addrinfo hints;
	   addrinfo* res;

	   memset(&hints, 0, sizeof(struct addrinfo));
	   hints.ai_flags 		= AI_PASSIVE;
	   hints.ai_family 		= AF_INET;
	   hints.ai_socktype 	= SOCK_STREAM;//SOCK_STREAM SOCK_DGRAM

	   if (0 != getaddrinfo(NULL, port.c_str(), &hints, &res))
	   {
	      cout << "illegal port number or port is busy.\n" << endl;
	      return 0;
	   }

	   serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	   if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
	   {
	      cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
	      return 0;
	   }

	   freeaddrinfo(res);

	   cout << "server is ready at port: " << port << endl;

	   UDT::listen(serv, 10);
	return 0;
}

int releaseServer() {
	   UDT::close(mSendSock);
	   // use this function to release the UDT library
	   UDT::cleanup();
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
		  cout << "before" << endl;
		  UDTSOCKET fhandle;
		  sockaddr_storage clientaddr;
		  int addrlen = sizeof(clientaddr);
	      if (UDT::INVALID_SOCK == (fhandle = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
	      {
	         cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
	         return 0;
	      }
	      cout << "after" << endl;
	      char clienthost[NI_MAXHOST];
	      char clientservice[NI_MAXSERV];
	      getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
	      cout << "new connection: " << clienthost << ":" << clientservice << endl;

	      #ifndef WIN32
	         pthread_t filethread;
	         pthread_create(&filethread, NULL, sendfile, new UDTSOCKET(fhandle));
	         pthread_detach(filethread);
	      #else
	         CreateThread(NULL, 0, sendfile, new UDTSOCKET(fhandle), 0, NULL);
	      #endif
	   }
}


int main(int argc, char* argv[])
{
	createServer(mSendSock, "9000");

	start(acceptEvent);

    getchar();

    stop();
    releaseServer();

    return 0;
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
      cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }

   if (UDT::ERROR == UDT::recv(fhandle, file, len, 0))
   {
      cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }
   file[len] = '\0';
   cout << "file len:" << len << " filename:" << file << endl;
   // open the file
   fstream ifs(file, ios::in | ios::binary);

   ifs.seekg(0, ios::end);
   int64_t size = ifs.tellg();
   ifs.seekg(0, ios::beg);

   // send file size information
   if (UDT::ERROR == UDT::send(fhandle, (char*)&size, sizeof(int64_t), 0))
   {
      cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }

   UDT::TRACEINFO trace;
   UDT::perfmon(fhandle, &trace);

   // send the file
   int64_t offset = 0;
   if (UDT::ERROR == UDT::sendfile(fhandle, ifs, offset, size))
   {
      cout << "sendfile: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }

   UDT::perfmon(fhandle, &trace);
   cout << "speed = " << trace.mbpsSendRate << "Mbits/sec" << endl;

   UDT::close(fhandle);

   ifs.close();

   #ifndef WIN32
      return NULL;
   #else
      return 0;
   #endif
}
