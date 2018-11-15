
#include <stdio.h>

#include "FileDeCodecJni.h"
#include "FileDeCodec.h"

void testDecodecJni() {
	FileDeCodecJni *codeJni = new FileDeCodecJni();
	codeJni->StartVideo(0);
	getchar();
	delete codeJni;
	codeJni = NULL;
}

void testFileDecodec() {
	FileDeCodec *codec = new FileDeCodec();
	codec->StartVideo();
	getchar();
	codec->StopVideo();
	delete codec;
	codec = NULL;
}

int main(int argc, char**argv) {

	testFileDecodec();
	return 0;
}
