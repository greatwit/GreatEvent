



#include "basedef.h"

#include "mediacodec.h"

#include "GMediaExtractor.h"

#include <unistd.h>



GMediaExtractor::GMediaExtractor()
	:mExtrator(NULL)
	,mFormat(NULL)
	,mbRunning(false)
{
	InitExtratorSymbols(&mSymbols);
}

GMediaExtractor::~GMediaExtractor()
{

}

void* GMediaExtractor::Thread() {
	GThread::ThreadStarted();
	GLOGW("FileDeCodec::Thread");

	uint8_t buffer[100000] = {0};
	ssize_t size = 0;
	do{
		size = mSymbols.AMediaExtractor.readSampleData(mExtrator, buffer, 100000);
		GLOGE("readSampleData size:%d", size);
		if(size>0)
			mSymbols.AMediaExtractor.advance(mExtrator);
		usleep(40*1000);
	}while(mbRunning && size>0);

	return 0;
}

int GMediaExtractor::startPlayer(const char*filepath) {
	int rest = 1;

	if (GThread::IsRunning())
		return 0;

	mExtrator = mSymbols.AMediaExtractor.newext();
	if(mExtrator) {
		media_status_t stat = mSymbols.AMediaExtractor.setDataSource(mExtrator, filepath);
		size_t count = mSymbols.AMediaExtractor.getTrackCount(mExtrator);
		GLOGE("get track count:%d err:%d", count, stat);

		for(int i=0;i<count;i++) {
			int32_t iout = 0;
			AMediaFormat*format = mSymbols.AMediaExtractor.getTrackFormat(mExtrator, i);
			mSymbols.AMediaFormat.getInt32(format, "width", &iout);
			GLOGE("AMediaFormat.getInt32 mime:%d", iout);
		}

		stat = mSymbols.AMediaExtractor.selectTrack(mExtrator, 0);
		GLOGE("selectTrack err:%d", stat);
		stat = mSymbols.AMediaExtractor.seekTo(mExtrator, 0, AMEDIAEXTRACTOR_SEEK_PREVIOUS_SYNC);
		GLOGE("seekTo err:%d", stat);


		if (GThread::Start() < 0)
			return -1;

		mbRunning = true;
	}
	return rest;
}

int GMediaExtractor::stopPlayer() {
	int rest = -1;

	mbRunning = false;

	if (GThread::IsRunning()) {
		GThread::Kill();
	}

	if(mExtrator) {
		mSymbols.AMediaExtractor.deleteext(mExtrator);
	}
	return rest;
}


