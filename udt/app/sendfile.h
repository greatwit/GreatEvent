#ifndef __SEND_FILE_H__
#define __SEND_FILE_H__

#include <fstream>
#include <iostream>
#include <cstring>
#include <udt.h>

using namespace std;

int createServer(UDTSOCKET &serv, string port);
int releaseServer();
pthread_t start(void *(*func_routine)(void*));
int stop();

#endif
