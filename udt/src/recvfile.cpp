
#ifndef WIN32
   #include <arpa/inet.h>
   #include <netdb.h>
#else
   #include <winsock2.h>
   #include <ws2tcpip.h>
#endif


#include "recvfile.h"


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
	      cout << "incorrect server/peer address. " << ipaddr << ":" << port << endl;
	      return -1;
	   }

	   // connect to the server, implict bind
	   if (UDT::ERROR == UDT::connect(sockHandle, peer->ai_addr, peer->ai_addrlen))
	   {
	      cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
	      return -1;
	   }

	   freeaddrinfo(peer);

	   return 0;
}

int releaseConnect(UDTSOCKET fhandle) {
	UDT::close(fhandle);
    // use this function to release the UDT library
    UDT::cleanup();
	return 0;
}

int startRecv(UDTSOCKET fhandle, char*needFile, char*saveFile) {
	   // send name information of the requested file
	   int len = strlen(needFile);

	   if (UDT::ERROR == UDT::send(fhandle, (char*)&len, sizeof(int), 0))
	   {
	      cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
	      return -1;
	   }

	   if (UDT::ERROR == UDT::send(fhandle, needFile, len, 0))
	   {
	      cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
	      return -1;
	   }

	   // get size information
	   int64_t size;

	   if (UDT::ERROR == UDT::recv(fhandle, (char*)&size, sizeof(int64_t), 0))
	   {
	      cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
	      return -1;
	   }

	   if (size < 0)
	   {
	      cout << "no such file " << needFile << " on the server\n";
	      return -1;
	   }
	   cout << "size:" << size << endl;


	   // receive the file
	   mrecvStream.open(saveFile, ios::out | ios::binary | ios::trunc);
	   int64_t recvsize;
	   int64_t offset = 0;

	   if (UDT::ERROR == (recvsize = UDT::recvfile(fhandle, mrecvStream, offset, size)))
	   {
	      cout << "recvfile: " << UDT::getlasterror().getErrorMessage() << endl;
	      return -1;
	   }
	   //outFst = ofs;
	return 0;
}

int stopRecv() {
	mrecvStream.close();
	return 0;
}

int startFileRecv(char*ipaddr, char*port, char*needFile, char*saveFile) {
	createConnect(mRecvSock, ipaddr, port);
	startRecv(mRecvSock, needFile, saveFile);

	return 0;
}

int stopFileRecv() {
	stopRecv();
	releaseConnect(mRecvSock);
	return 0;
}

#ifndef __ANDROID__
int main(int argc, char* argv[])
{
   if ((argc != 5) || (0 == atoi(argv[2])))
   {
      cout << "usage: recvfile server_ip server_port remote_filename local_filename" << endl;
      return -1;
   }

   startFileRecv(argv[1],argv[2],argv[3],argv[4]);

   getchar();

   stopFileRecv();

   return 0;
}
#endif

