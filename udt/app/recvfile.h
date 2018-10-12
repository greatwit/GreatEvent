#ifndef __RECV_FILE_H__
#define __RECV_FILE_H__

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <udt.h>

using namespace std;

int createConnect(UDTSOCKET &sockHandle, char*ipaddr, char*port);
int releaseConnect(UDTSOCKET fhandle);
int startRecv(UDTSOCKET fhandle, char*needFile, char*saveFile);
int stopRecv();

#endif
