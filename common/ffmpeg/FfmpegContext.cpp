

#include "FfmpegContext.h"


#ifdef 	__ANDROID__
#define	FILE_PATH	"/sdcard/w.h264"
#else
#define	FILE_PATH	"ftest.h264"
#endif


FfmpegContext::FfmpegContext( string filename )
			:mFilename(filename)
			,mFmt_ctx(NULL)
			,mPkgcall(NULL)
			,mbRunning(false)
{

	int ret = 0;

	//Input
	if ((ret = avformat_open_input(&mFmt_ctx, mFilename.c_str(), 0, 0)) < 0) {
		printf( "Could not open input file.");
		//return -1;
	}

	if ((ret = avformat_find_stream_info(mFmt_ctx, 0)) < 0) {
		printf( "Failed to retrieve input stream information");
		//return -1;
	}

	mwFile = fopen(FILE_PATH, "wb+");
	if(mwFile)
		printf( "open input file %s success.\n", FILE_PATH);

	mPool.initPara(2);
}

FfmpegContext::~FfmpegContext() {
	if(mwFile != NULL)
		fclose(mwFile);
}

void FfmpegContext::demuxFunc( void *arg ) {
	FfmpegContext* context = (FfmpegContext*)arg;
	printf("demux run:%d\n", context->getRun());
	int res = 0;
    while ((res = context->getPackageCall()) >= 0) {
    }
}

void FfmpegContext :: setPkgcall(IPkgCall* call) {
	mPkgcall = call;
}

bool FfmpegContext :: getRun() {
	return mbRunning;
}

bool FfmpegContext :: startDemux() {
	if(mbRunning == false) {
		mbRunning = true;
		mPool.dispatch( demuxFunc, this );
	}
	return true;
}

bool FfmpegContext :: stopDemux() {
	mbRunning = false;
	return true;
}

int FfmpegContext::getPackageCall() {

	if(mbRunning==false) return -1;

	AVPacket pkt;
	int ret = av_read_frame(mFmt_ctx, &pkt);
	if(ret >= 0) {
        AVPacket orig_pkt = pkt;
        if(mPkgcall) {
        	AVStream *stream = mFmt_ctx->streams[pkt.stream_index];
        	int frameType = stream->codec->codec_type;
        	mPkgcall->PkgSource(pkt, frameType);
        }
        av_packet_unref(&orig_pkt);
	}
	return ret;
}

int FfmpegContext::getPackageData(AVPacket &pkt, int &frameType) {

	int ret = av_read_frame(mFmt_ctx, &pkt);//0 is ok,<0 is error or end of file

    if (ret >= 0) {
    	AVStream *stream = mFmt_ctx->streams[pkt.stream_index];
    	frameType 		 = stream->codec->codec_type;
//		if (stream->codec->codec_type==AVMEDIA_TYPE_VIDEO ) {
//			char tag[4] = {0x00, 0x00, 0x00, 0x01};
//			fwrite(tag, 1, 4, mwFile);
//			fwrite(pkt.data+4, 1, pkt.size-4, mwFile);
//		}
    }
	return ret;
}

int FfmpegContext::getPlayInfo(PLAYER_INIT_INFO &playinfo, unsigned int &endTime) {

	int ret = 0;
	endTime = (mFmt_ctx->duration - mFmt_ctx->start_time)/1000;

	for (int i = 0; i < mFmt_ctx->nb_streams; i++) {
			//Create output AVStream according to input AVStream
			//AVFormatContext *ofmt_ctx;
			AVStream *pStream = mFmt_ctx->streams[i];
			AVCodecContext *pCodec = pStream->codec;
			//AVStream *out_stream = NULL;

			AVMediaType codecType = mFmt_ctx->streams[i]->codec->codec_type;
			if(codecType==AVMEDIA_TYPE_VIDEO) {
				//out_stream=avformat_new_stream(ofmt_ctx_v, in_stream->codec->codec);

				playinfo.nCodeID					= pCodec->codec_id+1;
				playinfo.nFps						= (pStream->r_frame_rate.den==0)?25:(pStream->r_frame_rate.num / pStream->r_frame_rate.den);
				playinfo.nWidth						= pCodec->width;
				playinfo.nHeigth					= pCodec->height;

				if((pCodec->extradata) && (pCodec->extradata_size>0)) {
					playinfo.nVideoExtSize			= pCodec->extradata_size;
					//int nCopy = pCodec->extradata_size <= sizeof(playinfo.videoExtData)?pCodec->extradata_size:sizeof(playinfo.videoExtData);
					//memcpy_s(playinfo.videoExtData, sizeof(playinfo.videoExtData), pCodec->extradata, nCopy);
					memcpy(playinfo.videoExtData, pCodec->extradata, pCodec->extradata_size);
				}

				printf("len:%d\n",pCodec->extradata_size);
				for(int i=0; i<pCodec->extradata_size; i++)
					printf("v:0x%02X\n", (unsigned char)pCodec->extradata[i]);

//				int spsLength=pCodec->extradata[6]*0xFF+pCodec->extradata[7];
//				int ppsLength=pCodec->extradata[8+spsLength+1]*0xFF+pCodec->extradata[8+spsLength+2];
//				printf( "spsLen:%d ppsLen:%d \n", spsLength, ppsLength);
//
//				char tag[4] = {0x00, 0x00, 0x00, 0x01};
//				fwrite(tag, 1, 4, mwFile);
//				fwrite(pCodec->extradata+8, 1, spsLength, mwFile);
//				fwrite(tag, 1, 4, mwFile);
//				fwrite(pCodec->extradata+8+spsLength+2+1, 1, ppsLength, mwFile);
			}
			else if(codecType==AVMEDIA_TYPE_AUDIO)
			{
				playinfo.nAudioFormat				= pCodec->codec_id;//86018;//
				//if(playinfo.nAudioFormat == AV_CODEC_ID_NONE || playinfo.nAudioFormat == AV_CODEC_ID_ADPCM_IMA_WAV)
				//	playinfo.nAudioFormat = AV_CODEC_ID_IMA;
				playinfo.nCodecFlag					= pCodec->codec_tag;
				playinfo.bit_rate					= pCodec->bit_rate;
				playinfo.bit_ratetolerance			= pCodec->bit_rate_tolerance;
				playinfo.sample_fmt					= pCodec->sample_fmt;
				playinfo.bits_per_sample			= pCodec->bits_per_coded_sample;//bit count
				playinfo.nBistspersample			= pCodec->bits_per_raw_sample;  // sample right 16
				if(playinfo.nBistspersample==0) {
					switch(pCodec->sample_fmt) {
						case AV_SAMPLE_FMT_U8:
						case AV_SAMPLE_FMT_U8P:
							playinfo.nBistspersample=8;
							break;
						case AV_SAMPLE_FMT_S16:
						case AV_SAMPLE_FMT_S16P:
							playinfo.nBistspersample=16;
							break;
						case AV_SAMPLE_FMT_S32:
						case AV_SAMPLE_FMT_S32P:
						case AV_SAMPLE_FMT_FLT:
						case AV_SAMPLE_FMT_FLTP:

						case AV_SAMPLE_FMT_S64:
						case AV_SAMPLE_FMT_S64P:
						case AV_SAMPLE_FMT_DBL:
						case AV_SAMPLE_FMT_DBLP:
							playinfo.nBistspersample=32;
							break;
						default:
							playinfo.nBistspersample=16;
							break;
					}
				}
				playinfo.block_align				= pCodec->block_align;
				playinfo.channel_layout				= pCodec->channel_layout;//unsigned long long
				playinfo.nSampleRate				= pCodec->sample_rate;
				playinfo.nChannel					= pCodec->channels;

				if(playinfo.nAudioFormat == AV_CODEC_ID_MP2) {
					playinfo.nAudioFormat = AV_CODEC_ID_MP1;
					if(playinfo.nSampleRate == 0) playinfo.nSampleRate = 8000;
					if(playinfo.nChannel == 0) playinfo.nChannel = 2;
				}

				playinfo.extsize					= pCodec->extradata_size;
				playinfo.gop_size					= pCodec->gop_size;
				playinfo.frame_size					= pCodec->frame_size;
				playinfo.frame_number				= pCodec->frame_number;
				playinfo.ildct_cmp					= pCodec->ildct_cmp;
				playinfo.me_subpel_quality			= pCodec->me_subpel_quality;
				playinfo.mb_lmax					= pCodec->mb_lmax;
				playinfo.mb_lmin					= pCodec->mb_lmin;
				playinfo.me_penalty_compensation	= pCodec->me_penalty_compensation;
				playinfo.qblur						= pCodec->qblur;	//float
				memcpy(playinfo.extdata, pCodec->extradata, pCodec->extradata_size);

				printf( "nAudioFormat:%d nSampleRate:%d nChannel:%d bits_per_sample:%d bit_rate:%d bit_ratetolerance:%d frame_size:%d sample_fmt:%d\n",
						playinfo.nAudioFormat, playinfo.nSampleRate, playinfo.nChannel, playinfo.bits_per_sample, playinfo.bit_rate,
						playinfo.bit_ratetolerance, playinfo.frame_size, playinfo.sample_fmt);
			}
	}

	return 0;
}

int FfmpegContext::getFileInfo(FILE_INFO &fileInfo) {

	getPlayInfo(fileInfo.pi, fileInfo.tmEnd);
	fileInfo.tmStart 	= 0;

	return 0;
}

#define AV_CODEC_FLAG_GLOBAL_HEADER (1 << 22)
#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER
#define AVFMT_RAWPICTURE 0x0020

int FfmpegContext::GetH264Stream()
{
   int ret;
   int videoindex = 0,audioindex = 0;
   AVFormatContext *ic=NULL;
   AVFormatContext *oc=NULL;

   uint8_t sps[100];
   uint8_t pps[100];
   int spsLength=0;
   int ppsLength=0;
   uint8_t startcode[4]={00,00,00,01};
   FILE *fp;

   fp=fopen("123.h264", "wb+");

   char *InputFileName="h264/tmp.mp4";
   char *OutPutPath = "out.h264";

   if ((ret = avformat_open_input(&ic, InputFileName, NULL, NULL)) < 0)
   {
       return ret;
   }

   if ((ret = avformat_find_stream_info(ic, NULL)) < 0)
   {
       avformat_close_input(&ic);
       return ret;
   }

   spsLength=ic->streams[0]->codec->extradata[6]*0xFF+ic->streams[0]->codec->extradata[7];

   ppsLength=ic->streams[0]->codec->extradata[8+spsLength+1]*0xFF+ic->streams[0]->codec->extradata[8+spsLength+2];

   for (int i=0;i<spsLength;i++)
   {
       sps[i]=ic->streams[0]->codec->extradata[i+8];
   }

   for (int i=0;i<ppsLength;i++)
   {
       pps[i]=ic->streams[0]->codec->extradata[i+8+2+1+spsLength];
   }


   for(int i=0;i<ic->nb_streams;i++)
   {
       if(ic->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
       {
           videoindex=i;
       }
       else if(ic->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
       {
           audioindex=i;
       }
   }

   AVOutputFormat *ofmt = NULL;
   AVPacket pkt;

   avformat_alloc_output_context2(&oc, NULL, NULL, OutPutPath);

   if (!oc)
   {
       printf( "Could not create output context\n");
       ret = AVERROR_UNKNOWN;
   }
   ofmt = oc->oformat;
   int i;

   for (i = 0; i < ic->nb_streams; i++)
   {
       AVStream *in_stream = ic->streams[i];
       AVStream *out_stream = avformat_new_stream(oc, in_stream->codec->codec);

       if (!out_stream)
       {
           printf( "Failed allocating output stream\n");
           ret = AVERROR_UNKNOWN;
       }
       ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
       if (ret < 0)
       {
           printf( "Failed to copy context from input to output stream codec context\n");
       }
       out_stream->codec->codec_tag = 0;
       if (oc->oformat->flags & AVFMT_GLOBALHEADER)
           out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
   }

   if (!(ofmt->flags & AVFMT_NOFILE))
   {
       ret = avio_open(&oc->pb, OutPutPath, AVIO_FLAG_WRITE);
       if (ret < 0)
       {
           printf( "Could not open output file '%s'", OutPutPath);

       }
   }
   ret = avformat_write_header(oc, NULL);

   int frame_index=0;
   int flag=1;

   av_init_packet(&pkt);
   pkt.data = NULL;
   pkt.size = 0;

   while (1)
   {
       AVStream *in_stream, *out_stream;

       ret = av_read_frame(ic, &pkt);

       if (ret < 0)
           break;
       in_stream  = ic->streams[pkt.stream_index];
       out_stream = oc->streams[pkt.stream_index];

       AVPacket tmppkt;
       if (in_stream->codec->codec_type==AVMEDIA_TYPE_VIDEO )
       {
           if (flag)
           {
               fwrite(startcode,4,1,fp);
               fwrite(sps,spsLength,1,fp);
               fwrite(startcode,4,1,fp);
               fwrite(pps,ppsLength,1,fp);

               pkt.data[0]=0x00;
               pkt.data[1]=0x00;
               pkt.data[2]=0x00;
               pkt.data[3]=0x01;
               fwrite(pkt.data,pkt.size,1,fp);

               flag=0;
           }
           else
           {
               pkt.data[0]=0x00;
               pkt.data[1]=0x00;
               pkt.data[2]=0x00;
               pkt.data[3]=0x01;
               fwrite(pkt.data,pkt.size,1,fp);
           }

           pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
           pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
           pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
           pkt.pos = -1;

           pkt.stream_index=0;
           ret = av_interleaved_write_frame(oc, &pkt);
       }
       av_free_packet(&pkt);
   }

   fclose(fp);
   fp=NULL;

   av_write_trailer(oc);
   return 0;
}

