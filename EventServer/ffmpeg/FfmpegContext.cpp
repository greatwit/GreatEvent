

#include "FfmpegContext.h"





FfmpegContext::FfmpegContext( string filename )
			:mFilename(filename)
			,mFmt_ctx(NULL)
{
	av_register_all();
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

}

FfmpegContext::~FfmpegContext() {

}


int FfmpegContext::getFrameData() {
	int count = 0;
	AVPacket pkt;
    while (av_read_frame(mFmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        count++;
        printf("pkg count:%d len:%d type:%d\n", count, pkt.size, pkt.flags);
//        do {
//            ret = decode_packet(&got_frame, 0);
//            if (ret < 0)
//                break;
//            pkt.data += ret;
//            pkt.size -= ret;
//        } while (pkt.size > 0);
        av_packet_unref(&orig_pkt);
    }
	return 0;
}

int FfmpegContext::getPackageData(AVPacket &pkt) {
	return av_read_frame(mFmt_ctx, &pkt);
}

/*
typedef struct tagPLAYER_INIT_INFO
{
	int nCodeID;
	int nFps;
	int nWidth;
	int nHeigth;
	int nSampleRate;
	int nBistspersample;
	int nChannel;
	int nAudioFormat;
	int sample_fmt;
	unsigned long long channel_layout;
	int nCodecFlag;
	int bits_per_sample;
	int bit_rate;
	int me_method;
	int bit_ratetolerance;
	int block_align;

	int gop_size;
	int frame_size;
	int frame_number;
	int ildct_cmp;
	int me_subpel_quality;
	int mb_lmax;
	int mb_lmin;
	int me_penalty_compensation;
	float qblur;
	int  flags;
	int extsize;
	char extdata[ARRAY_NUM];
	int  nVideoExtSize;
	char	videoExtData[ARRAY_NUM];
}
*/
int FfmpegContext::getPlayInfo(PLAYER_INIT_INFO &playinfo) {

	int ret = 0;
    //AVFormatContext *ifmt_ctx = NULL;


	playinfo.gop_size = mFmt_ctx->duration;

	for (int i = 0; i < mFmt_ctx->nb_streams; i++) {
			//Create output AVStream according to input AVStream
			AVFormatContext *ofmt_ctx;
			AVStream *in_stream = mFmt_ctx->streams[i];
			AVCodecContext *pCodec = in_stream->codec;
			AVStream *out_stream = NULL;

			AVMediaType codecType = mFmt_ctx->streams[i]->codec->codec_type;
			if(codecType==AVMEDIA_TYPE_VIDEO) {

				//out_stream=avformat_new_stream(ofmt_ctx_v, in_stream->codec->codec);

				playinfo.nCodeID					= pCodec->codec_id;
				playinfo.nFps						= in_stream->r_frame_rate.num / in_stream->r_frame_rate.den;
				playinfo.nWidth						= pCodec->width;
				playinfo.nHeigth					= pCodec->height;

				playinfo.nBistspersample			= pCodec->bits_per_coded_sample;

				playinfo.sample_fmt					= pCodec->sample_fmt;
				playinfo.nCodecFlag					= pCodec->codec_tag;
				playinfo.me_method;
				playinfo.bit_ratetolerance			= pCodec->bit_rate_tolerance;


				playinfo.frame_size					= pCodec->frame_size;
				playinfo.frame_number				= pCodec->frame_number;
				playinfo.ildct_cmp					= pCodec->ildct_cmp;
				playinfo.me_subpel_quality			= pCodec->me_subpel_quality;
				playinfo.mb_lmax					= pCodec->mb_lmax;
				playinfo.mb_lmin					= pCodec->mb_lmin;;
				playinfo.me_penalty_compensation	= pCodec->me_penalty_compensation;
				playinfo.qblur						= pCodec->qblur;//float
				playinfo.flags						= pCodec->flags;
				playinfo.extsize					= pCodec->extradata_size;
				playinfo.extdata[ARRAY_NUM];
				playinfo.nVideoExtSize;

				playinfo.bits_per_sample			= pCodec->bits_per_raw_sample;
				playinfo.bit_rate					= pCodec->bit_rate;

			}
			else if(codecType==AVMEDIA_TYPE_AUDIO)
			{
				playinfo.nAudioFormat				= in_stream->codecpar->format;
				playinfo.nSampleRate				= pCodec->sample_rate;
				playinfo.nChannel					= pCodec->channels;
				playinfo.channel_layout				= pCodec->channel_layout;//unsigned long long
				playinfo.block_align				= in_stream->codecpar->block_align;
			}
	}

	return 0;
}

int FfmpegContext::getFileInfo(FILE_INFO &fileInfo) {

	getPlayInfo(fileInfo.pi);
	fileInfo.tmEnd 		= 100000000;
	fileInfo.tmStart 	= 0;

	return 0;
}

