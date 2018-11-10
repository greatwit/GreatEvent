#ifndef FILE_DE_CODEC_JNI_H
#define FILE_DE_CODEC_JNI_H


#include <string>
#include <map>

#include "IPkgCall.h"

class FfmpegContext;

class FileDeCodecJni : public IPkgCall
{
	public:
	FileDeCodecJni();
		virtual ~FileDeCodecJni();

		//bool CreateCodec(const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, char*filename);
		//bool CreateCodec(const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, char*filename);
		bool DeInit();
		bool StartVideo(int deivceid);
		bool StopVideo();

	private:
		void PkgSource(AVPacket&pkt, int &frameType);
		
//	protected:
//		bool isFirstFrame();
//		void setFirstFrame(bool bFirs);
//
//		void decorder(char*data, int dataLen);
//		virtual bool threadLoop();
//
//		void onCodecBuffer(struct CodecBuffer& buff);
//
//	private:
//		bool				mFirstFrame;
		bool				mbRunning;
		FfmpegContext 		*mFfmpegContext;
//
//		FILE 				*mFile;
//
//		char mcharLength[4];
//		char mData[1000000];
		
		//sp<CodecBase> 	mCodec;
};

#endif


