
#include "basedef.h"

#define TAG "FileDeCodec"
#include "FileDeCodec.h"
#include <unistd.h>


//char *gFilePath1 = "/sdcard/camera.h264";
#ifdef 	__ANDROID__
#define	FILE_PATH	"/sdcard/uptest.h264"
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
			,mCount(0)
{
	GLOGW("FileDeCodec::FileDeCodec() construct.");
	mFile = OpenBitstreamFile( FILE_PATH );//fopen(FILE_PATH, "rb");
	GLOGW("open filepath:%s", FILE_PATH);
	if(mFile==NULL)
		GLOGE("OpenBitstreamFile failed.");

	if(MC_API_ERROR == MediaCodecNdk_Init(&mApi))
		GLOGE("MediaCodecNdk_Init error.");

	mApi.configure(&mApi, 0);
	mPool.initPara(2);
	mNALU  = AllocNALU(100000);
}

FileDeCodec::~FileDeCodec()
{
	GLOGV("FileDeCodec, Destructor");
}

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

	mApi.set_output_surface(&mApi, surface, 0);

	union mc_api_args args;
	args.video.p_surface = surface;
	args.video.i_width 	= 1280;
	args.video.i_height = 720;
	args.video.i_angle 	= 0;
	int err = mApi.start(&mApi, &args);
	GLOGE("function %s,line:%d mApi.start res:%d",__FUNCTION__,__LINE__,err);

	if (GThread::Start() < 0)
		return false;

	mbRunning = true;
	mPool.dispatch( dequeueFunc, this );

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

void FileDeCodec::dequeueFunc( void *arg ) {
	int res = 0;
	FileDeCodec* context = (FileDeCodec*)arg;
	context->renderBuffer();
}

void FileDeCodec::renderBuffer() {
	//mc_api_out mcout;
	//err = mApi.get_out(&mApi, index, &mcout);//GetOutput(mc_api *api, int i_index, mc_api_out *p_out)

	int err = 0;
	while(mbRunning) {

		if(mCount>0) {
			int index = mApi.dequeue_out(&mApi, 12000);
			if(index>=0)
				mCount--;
			mc_api_out mcout;
			int rest = mApi.get_out(&mApi, index, &mcout);
			GLOGI("renderBuffer---------rest:%d",rest);
//			if(index>=0) {
//				err = mApi.release_out(&mApi, index, true);
//				GLOGW("release_out err:%d", err);
//				mCount--;
//			} else {
//				GLOGE("dequeue_out index:%d", index);
//				mc_api_out mcout;
//				int rest = mApi.get_out(&mApi, index, &mcout);
//			}
		}
		usleep(20*1000);
	}
}

void * FileDeCodec::Thread()
{
	GThread::ThreadStarted();
	GLOGW("FileDeCodec::Thread");

	int err=0;
	long timeoutUs = 10000;
	do{
		int index = mApi.dequeue_in(&mApi, timeoutUs);
		GLOGW("dequeue_in index:%d", index);
		if(index>=0) {
			int size=GetAnnexbNALU(mFile, mNALU);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
			GLOGE("GetAnnexbNALU size:%d", size);
			if(size<4) {
				GLOGE("get nul error!\n");
				continue;
			}
			err = mApi.queue_in(&mApi, index, mNALU, size, 10000, false);
			GLOGW("queue_in err:%d", err);
			if(err>=0)
				mCount++;
			usleep(25*1000);
		}else
			continue;

	}while(!feof(mFile)&&mbRunning);

	mbRunning = false;

//	int res = 0, dataLen = 0, err=0;
//	int count = 0;
//	do {
//		res = fread(mcharLength, 4, 1, mFile);
//		if(res>0)
//		{
//			long timeoutUs = 10000;
//			dataLen = charsToInt1(mcharLength, 0);
//			res 	= fread(mData, dataLen, 1, mFile);
//			count++;
//			GLOGW("count:%d res:%d dataLen:%d", count, res, dataLen);
//
//			int index = mApi.dequeue_in(&mApi, timeoutUs);
//			GLOGW("dequeue_in index:%d", index);
//			if(index>=0) {
//				err = mApi.queue_in(&mApi, index, mData, dataLen, 1000000, true);
//				GLOGW("queue_in err:%d", err);
//			}else
//				continue;
//			mCount++;
//			usleep(20*1000);
//
//		}else
//			break;
//	}while(mbRunning);

	return 0;
}



