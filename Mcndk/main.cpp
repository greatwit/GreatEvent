
#include <stdio.h>

#include "FileDeCodecJni.h"

int main(int argc, char**argv) {
	FileDeCodecJni *codeJni = new FileDeCodecJni();
	codeJni->StartVideo(0);
	getchar();
	delete codeJni;
	codeJni = NULL;

	return 0;
}
