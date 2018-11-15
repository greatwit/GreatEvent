#ifndef FILE_DE_CODEC_H
#define FILE_DE_CODEC_H


#include "gthread.h"
#include <iostream>

#include "mediacodec.h"

class FileDeCodec  : private GThread
{
	public:
		FileDeCodec();
		virtual ~FileDeCodec();

		//bool CreateCodec(const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, char*filename);
		//bool CreateCodec(const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, char*filename);
		bool DeInit();
		bool StartVideo(void *surface);
		bool StopVideo();

		
	protected:
		bool isFirstFrame();
		void setFirstFrame(bool bFirs);

		void decorder(char*data, int dataLen);
		void *Thread();


	private:
		bool				mFirstFrame;
		bool				mbRunning;

		FILE 				*mFile;
		
		char mcharLength[4];
		char mData[1000000];
		
		struct mc_api mApi;

		//sp<CodecBase> 	mCodec;
};

#endif


