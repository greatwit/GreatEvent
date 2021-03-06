#ifndef __FFMPEG_CONTEXT_hpp__
#define __FFMPEG_CONTEXT_hpp__

#include <stdio.h>
#include <unistd.h>
#include <string>

#include "protocol.h"
#include "gthreadpool.hpp"

#include "IPkgCall.h"

#ifdef __cplusplus
extern "C"
{
	#endif
	#include "libavformat/avformat.h"
	#ifdef __cplusplus
};
#endif


using namespace std;

class FfmpegContext {
	public:
		FfmpegContext( string filename );
		virtual ~FfmpegContext();

		int getPlayInfo(PLAYER_INIT_INFO &playinfo, unsigned int &endTime);
		int getFileInfo(FILE_INFO &fileInfo);
		int getPackageCall();
		int getPackageData(AVPacket&pkt,int &frameType);
		int seekFrame(unsigned int mlSecond);

		int parseExtraDataMp4(AVCodecContext *pCont, const char*szFitler);
		int changeExtraData(AVCodecContext *pCont);

		bool startDemux();
		bool stopDemux();

		int GetH264Stream();
		bool getRun();
		void setPkgcall(IPkgCall* call);

	private:
		static void demuxFunc( void *arg );
		int getHDAWavDuration(const char* filename);

		string mFilename;
		AVFormatContext *mFmt_ctx;
		GThreadPool 	mPool;
		IPkgCall		*mPkgcall;
		bool 			mbRunning;
		bool			mbWavFile;
		int 			mIndex;
		int 			mAudioFormat;
};


#endif


