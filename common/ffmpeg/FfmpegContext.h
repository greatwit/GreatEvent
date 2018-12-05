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

		bool startDemux();
		bool stopDemux();

		int GetH264Stream();
		bool getRun();
		void setPkgcall(IPkgCall* call);

	private:
		static void demuxFunc( void *arg );

		string mFilename;
		AVFormatContext *mFmt_ctx;
		FILE			*mwFile;
		GThreadPool 	mPool;
		IPkgCall		*mPkgcall;
		bool 			mbRunning;
		int 			mIndex;
};


#endif


