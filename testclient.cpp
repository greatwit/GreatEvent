#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "spioutils.hpp"

int main( int argc, char * argv[] )
{
	int sock = 0;
	int ret = SP_IOUtils::tcpConnect("192.168.100.75", 8080, &sock, 0);
	printf("ret:%d sock:%d\n",ret, sock);
	char*data = "1234567890ab";
	ret = SP_IOUtils::tcpSendData(sock, data, 12);
	printf("ret:%d \n", ret);
	getchar();

	close(sock);

	return 0;
}
