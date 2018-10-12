#ifndef __RECV_FILE_H__
#define __RECV_FILE_H__

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "udt.h"

using namespace std;

int startFileRecv(char*ipaddr, char*port, char*needFile, char*saveFile);
int stopFileRecv();

#endif
