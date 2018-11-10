
#include "FileDeCodecJni.h"

#include "basedef.h"
#include "FfmpegContext.h"

#define TAG "FileDeCodecJni"

//char *gFilePath1 = "/sdcard/camera.h264";


FileDeCodecJni::FileDeCodecJni()
			:mbRunning(false)
			//,mFfmpegContext(NULL)
{
	//ALOGV("FileDeCodec::FileDeCodec() construct.");
	mFfmpegContext = new FfmpegContext("tmp.mp4");
}

FileDeCodecJni::~FileDeCodecJni()
{
	GLOGV("FileDeCodec, Destructor");
}


//bool FileDeCodecJni::CreateCodec( const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, char*filename)
//{
//	mFile = fopen(filename, "rb");
//	mcharLength[4] = {0};
//	mData[1000000] = {0};
//
//	//mCodec = new CodecBase("video/avc", true, false);
//	//mCodec->CreateCodec(format, surface, crypto, flags);
//	CodecBaseLib::getInstance()->CodecCreate(format, surface, crypto, flags, false);
//
//	return true;
//}

bool FileDeCodecJni::DeInit()
{	
	if(mbRunning)
		StopVideo();
	
	//fclose(mFile);
	
	return true;
}

void FileDeCodecJni::PkgSource(AVPacket&pkt, int &frameType) {
	printf("type:%d len:%d\n", frameType, pkt.size);
}

bool FileDeCodecJni::StartVideo(int deivceid)
{
	if(mbRunning)
		return false;

	mFfmpegContext->setPkgcall(this);
	mFfmpegContext->startDemux();

//	//mCodec->startCodec();
//	CodecBaseLib::getInstance()->StartCodec();
//
//	GLOGD("function %s,line:%d",__FUNCTION__,__LINE__);

	mbRunning = true;  

	return true;
}

bool FileDeCodecJni::StopVideo()
{
	if(false==mbRunning)
		return false;

	GLOGW("function %s,line:%d StopVideo 0",__FUNCTION__,__LINE__);

//	mbRunning = false;
//	requestExit();
//
//	//mCodec->stopCodec();
//	CodecBaseLib::getInstance()->StopCodec();
//
//	GLOGD("function %s,line:%d StopVideo 2",__FUNCTION__,__LINE__);

	return true;
}



