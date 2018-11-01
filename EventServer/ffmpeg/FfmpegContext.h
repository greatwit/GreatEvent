#ifndef __FFMPEG_CONTEXT_hpp__
#define __FFMPEG_CONTEXT_hpp__

#include <stdio.h>
#include <unistd.h>
#include <string>

#include "protocol.h"


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

	int getPlayInfo(PLAYER_INIT_INFO &playinfo);
	int getFileInfo(FILE_INFO &fileInfo);
	int getFrameData();
	int getPackageData(AVPacket&pkt);

private:
	string mFilename;
	AVFormatContext *mFmt_ctx;

};


#endif
