#ifndef __SEND_FILE_H__
#define __SEND_FILE_H__

#include <fstream>
#include <iostream>
#include <cstring>
#include "udt.h"

using namespace std;

#ifdef __cplusplus
	  extern "C"{
#endif

	int startFileSend(char*port);
	int stopFileSend();

#ifdef __cplusplus
}
#endif

#endif
