#include "UdtSender.h"

#include "sendvideo.h"
#include "basedef.h"
#include "h264.h"

	UdtSender::UdtSender() {

	}

	UdtSender::~UdtSender() {

	}

	int	UdtSender::create(short localPort) {
		return 0;
	}

    int	UdtSender::release() {
    	return 0;
    }

    int UdtSender::connect(char* sDestIp, short destPort) {
    	int ret = 0;
    	ret = connect_(sDestIp, destPort);
        return ret;
    }

	int UdtSender::disConnect() {
		return disConnect_();
	}

	int UdtSender::sendBuffer(void*buff, int dataLen, int64_t timeStamp) {
		return sendData((char*)buff, dataLen);
	}

	int UdtSender::getSockHandle() {
		return getSockHandle_();
	}

	void* UdtSender::sendfileProc(void* sender)
	{
		UdtSender *pSender = (UdtSender*)sender;
		int sockHand = pSender->getSockHandle();

		FILE			*mwFile = OpenBitstreamFile( "camera_640x480.h264" );
		NALU_t 			*mNALU  = AllocNALU(8000000);
		int ret = 0;
		UDT::TRACEINFO trace;
		UDT::perfmon(sockHand, &trace);

		while(true) {
			if(feof(mwFile)) {
				//mRunning = false;
				GLOGW("read file done.");
				break;
			}
			int size=GetAnnexbNALU(mwFile, mNALU);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
			//GLOGE("GetAnnexbNALU size:%d", n->len);
			if(size<4){
				GLOGE("get nul error!\n");
				break;
			}

			ret = pSender->sendBuffer( (char*)mNALU->buf, mNALU->len, 0 );
			GLOGE("ret:%d len:%d ", ret, mNALU->len);
		}

		UDT::perfmon(sockHand, &trace);
		GLOGE("speed = %f Mbits/sec ", trace.mbpsSendRate);

		//UDT::close(sockHand);

	    if(mwFile != NULL)
			fclose(mwFile);

		FreeNALU(mNALU);

	    return NULL;
	}


#ifndef __ANDROID__
int main(int argc, char* argv[])
{
   if ((argc != 3) || (0 == atoi(argv[2])))
   {
      cout << "usage: sendvideo server_ip server_port" << endl;
      return -1;
   }

   UdtSender *sender = new UdtSender();
   sender->connect(argv[1], atoi(argv[2]));

   pthread_t recvthread;
   pthread_create(&recvthread, NULL, UdtSender::sendfileProc, sender);
   pthread_detach(recvthread);

   getchar();

   sender->disConnect();
   delete sender;
   sender = NULL;

   return 0;
}
#endif
