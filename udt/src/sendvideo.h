#ifndef __SEND_VIDEO_H__
#define __SEND_VIDEO_H__

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "udt.h"

using namespace std;

int startVideoSend(char*ipaddr, char*port, char*needFile, char*saveFile);
int stopVideoSend();

#endif
