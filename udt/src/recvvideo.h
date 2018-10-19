#ifndef __RECV_VIDEO_H__
#define __RECV_VIDEO_H__

#include <fstream>
#include <iostream>
#include <cstring>
#include "udt.h"

#include "IReceiveCallback.h"


using namespace std;

int startViedoRecv(char*port);
int stopViedoRecv();
void registerCb(IReceiveCallback *base);

#endif
