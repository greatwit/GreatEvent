
#include <stdio.h>

#include "NetDataClient.h"
#include "PlayBackClient.h"

static void onDownload(HANDLE hDownload,const char* szSrcFile,const char* szDstFile,DWORD dwPos,DWORD dwTotal,void* param){
	//todo for download file

}
static void onAVdata(void* param,LPAV_FRAME lpFrame,BOOL dataEnd,BOOL bSend){

//	JNIEnv* envp;
//	jint ret = gVM->GetEnv(reinterpret_cast<void**>(&envp), JNI_VERSION_1_4);
//	ret = gVM->AttachCurrentThread(&envp,NULL);
//
//	jbyteArray dat = NULL;
//	if(lpFrame&&lpFrame->nLength>0){
//	    dat = envp->NewByteArray(lpFrame->nLength);
//		envp->SetByteArrayRegion(dat,0,lpFrame->nLength,(jbyte*)lpFrame->lpData);
//	}
//	if(dat){
//		envp->CallVoidMethod(mobject,mthAV,(int)param,lpFrame->dwFrameType,lpFrame->dwTick,lpFrame->dwTm,dat,dataEnd,bSend);
//		envp->DeleteLocalRef(dat);
//	}else
//		envp->CallVoidMethod(mobject,mthAV,(int)param,0,0,0,NULL,dataEnd,bSend);
//
//	ret = gVM->DetachCurrentThread();

	LPLOGIN_RET lpRet = (LPLOGIN_RET)lpFrame->lpData;
	printf("recv len:%d retlen:%d\n", lpFrame->nLength, lpRet->nLength);

	return;
}

static int getEmptyBufSize(void* param){
//
//	JNIEnv* envp;
//		jint ret = gVM->GetEnv(reinterpret_cast<void**>(&envp), JNI_VERSION_1_4);
//		ret = gVM->AttachCurrentThread(&envp,NULL);
//		ret = envp->CallIntMethod(mobject,methodid,(int)param);
//		gVM->DetachCurrentThread();
//		return ret;

	return 0;
}

int main(int argc, char**argv)
{
	if ((argc != 3)) {
		printf("Usage: app server_ip local_filename.\n");
		return 0;
	}

	int param = 0;
	CFILE_INFO info;
	memset(&info, 0, sizeof(CFILE_INFO));
	info.param = (void*)param;
	if(true) {
		info.proc = onDownload;
		info.onav = onAVdata;
		info.getbufSize = getEmptyBufSize;
	}else{
		//info.onfile = onFileData;
	}

    CNetDataClient *lpClient = new CPlayBackClient();
    int ret = lpClient->OpenPlayback(argv[1], 8080, argv[2], NULL, &info); //now connect to the server

    getchar();

    lpClient->StartWork();

    getchar();
    lpClient->Start(0, 10000);

    getchar();
    lpClient->CloseClient();
    delete lpClient;
    lpClient = NULL;

    return 0;
}




