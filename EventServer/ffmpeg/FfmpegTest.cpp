//#include "FfmpegContext.h"
#include <stdio.h>

#include "FfmpegContext.h"

#define FILE_NAME "h264/tmp1.mp4"


int main(int argc, char**argv) {

	char lpRet[sizeof(NET_CMD) + sizeof(LOGIN_RET)];
	ZeroMemory(lpRet,sizeof(lpRet));

	((LPNET_CMD)lpRet)->dwFlag = NET_FLAG;
	((LPNET_CMD)lpRet)->dwCmd = MODULE_MSG_LOGINRET;
	((LPNET_CMD)lpRet)->dwLength = sizeof(LOGIN_RET);
	LPLOGIN_RET lpLR = (LPLOGIN_RET)((LPNET_CMD)lpRet)->lpData;
	lpLR->nLength = sizeof(lpLR->lpData);

	//printf("((LPNET_CMD)lpRet)->dwLength len:%d LOGIN_RET data len:%d \n", ((LPNET_CMD)lpRet)->dwLength, lpLR->nLength);
	if(argc!=2) {
		printf("usage:exe filepath.\n");
		return 0;
	}
	else
		printf("filename:%d\n", argv[1]);
//	lpLR->lRet = m_pManager->GetResult(MODULE_MSG_LOGIN,
//		((LPNET_CMD)lpData)->lpData,
//		((LPNET_CMD)lpData)->dwLength,
//		lpLR->lpData,&lpLR->nLength);


	LPFILE_INFO	  fileInfo = (LPFILE_INFO)lpLR->lpData;
	FfmpegContext *context = new FfmpegContext(argv[1]);
	context->getFileInfo(*fileInfo);
//
	PLAYER_INIT_INFO &playInfo = fileInfo->pi;
	printf("w:%d h:%d size:%d framerate:%d\n",
			playInfo.nWidth, playInfo.nHeigth,
			playInfo.gop_size, playInfo.nFps);
//
	printf("nAudioFormat:%d nChannel:%d nSampleRate:%d bit_rate:%d\n",
			playInfo.nAudioFormat, playInfo.nChannel,
			playInfo.nSampleRate, playInfo.bit_rate);

	//memcpy(lpLR->lpData, (void*)fileInfo, sizeof(fileInfo));

	getchar();

	//context->getFrameData();
	AVPacket pkt;
	int count = 0;
	int res = 0;
	int fType = -1;
    while ((res = context->getPackageData(pkt, fType)) >= 0) {
        AVPacket &orig_pkt = pkt;
        count++;
        if(fType == 0)
        printf("pkg count:%d len:%d type:%d flag:%d\n", count, pkt.size, fType, pkt.flags);

        av_packet_unref(&orig_pkt);
    }

    if(res == AVERROR_EOF)
    	printf("eof.\n");

	getchar();
	//context->GetH264Stream();
	//getchar();
	delete context;
	context = NULL;

	return 0;
}
