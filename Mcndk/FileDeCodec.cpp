
#include "basedef.h"

#define TAG "FileDeCodec"
#include "FileDeCodec.h"
#include <unistd.h>


//char *gFilePath1 = "/sdcard/camera.h264";
#ifdef 	__ANDROID__
#define	FILE_PATH	"/sdcard/camera.h264"
#else
#define	FILE_PATH	"camera.h264"
#endif

int charsToInt1(char* src, int offset) 
{  
	int value;
	value = (int) ((src[offset]	  & 0xFF)
				| ((src[offset+1] & 0xFF)<<8)
				| ((src[offset+2] & 0xFF)<<16)
				| ((src[offset+3] & 0xFF)<<24));
	return value;
}

FileDeCodec::FileDeCodec()
			:mbRunning(false)
			,mFirstFrame(false)
			,mFile(NULL)
{
	GLOGW("FileDeCodec::FileDeCodec() construct.");
	mFile = fopen(FILE_PATH, "rb");
	GLOGW("open filepath:%s", FILE_PATH);
	if(MC_API_ERROR == MediaCodecNdk_Init(&mApi))
		GLOGE("MediaCodecNdk_Init error.");

	mApi.configure(&mApi, 0);
}

FileDeCodec::~FileDeCodec()
{
	GLOGV("FileDeCodec, Destructor");
}

//bool FileDeCodec::CreateCodec( const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, char*filename)
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

bool FileDeCodec::DeInit()
{	
	StopVideo();
	
	if(mFile)
		fclose(mFile);
	
	return true;
}

bool FileDeCodec::StartVideo(void *surface)
{
	if (GThread::IsRunning())
		return false;

	if (GThread::Start() < 0)
		return false;

	mApi.set_output_surface(&mApi, surface, 0);

	union mc_api_args args;
	args.video.i_width 	= 1280;
	args.video.i_height = 720;
	args.video.i_angle 	= 0;
	int err = mApi.start(&mApi, &args);
	GLOGE("function %s,line:%d mApi.start res:%d",__FUNCTION__,__LINE__,err);
	mbRunning = true;  

	return true;
}

bool FileDeCodec::StopVideo()
{
	GLOGW("function %s,line:%d StopVideo 0",__FUNCTION__,__LINE__);


	if (GThread::IsRunning()) {
		GThread::Kill();
	}

	mbRunning = false;

	mApi.stop(&mApi);

	//mCodec->stopCodec();
	//CodecBaseLib::getInstance()->StopCodec();
	
	GLOGD("function %s,line:%d StopVideo 2",__FUNCTION__,__LINE__);

	return true;
}

void * FileDeCodec::Thread()
{
	GThread::ThreadStarted();
	GLOGW("FileDeCodec::Thread");
	int res = 0, dataLen = 0, err=0;
	int count = 0;
	do {
		res = fread(mcharLength, 4, 1, mFile);
		if(res>0)
		{
			dataLen = charsToInt1(mcharLength, 0);
			res 	= fread(mData, dataLen, 1, mFile);
			count++;
			GLOGW("count:%d res:%d dataLen:%d", count, res, dataLen);

			int index = mApi.dequeue_in(&mApi, 0);
			GLOGW("dequeue_in index:%d", index);
			err = mApi.queue_in(&mApi, index, mData, dataLen, -1, true);
			GLOGW("queue_in err:%d", err);
			//CodecBaseLib::getInstance()->AddBuffer(mData, dataLen);
			//mCodec->addBuffer(mData, dataLen);
			usleep(30*1000);
			index = mApi.dequeue_out(&mApi, 0);
			GLOGW("dequeue_out index:%d", index);
			mc_api_out mcout;
			err = mApi.get_out(&mApi, index, &mcout);//GetOutput(mc_api *api, int i_index, mc_api_out *p_out)
			GLOGW("get_out err:%d", err);
			mApi.release_out(&mApi, index, true);

		}else
			break;
	}while(mbRunning);

	return 0;
}



