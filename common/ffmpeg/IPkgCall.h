#ifndef I_PKG_CALLBACK
#define I_PKG_CALLBACK

struct AVPacket;

class IPkgCall
{
	public:
		virtual ~IPkgCall(){}
		virtual void PkgSource(AVPacket&pkt,int &frameType){}
};

#endif
