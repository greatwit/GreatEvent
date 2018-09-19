/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <sys/uio.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>

#include "spiochannel.hpp"

#include "speventcb.hpp"
#include "sputils.hpp"
//#include "spresponse.hpp"
#include "spsession.hpp"
#include "spbuffer.hpp"
//#include "spmsgblock.hpp"

#include "config.h"
#include "event.h"

#include "h264.h"

//---------------------------------------------------------

SP_IOChannel :: ~SP_IOChannel()
{
}

struct evbuffer * SP_IOChannel :: getEvBuffer( SP_Buffer * buffer )
{
	return buffer->mBuffer;
}

int SP_IOChannel :: transmit( SP_Session * session )
{
#ifdef IOV_MAX
	const static int SP_MAX_IOV = IOV_MAX;
#else
	const static int SP_MAX_IOV = 8;
#endif

	return 0;//len;
}

//---------------------------------------------------------

SP_IOChannelFactory :: ~SP_IOChannelFactory()
{
}

//---------------------------------------------------------

SP_DefaultIOChannel :: SP_DefaultIOChannel()
	:mPackHeadLen(sizeof(PACK_HEAD))
{
	mFd = -1;
}

SP_DefaultIOChannel :: ~SP_DefaultIOChannel()
{
	mFd = -1;
}

int SP_DefaultIOChannel :: init( int fd )
{
	mFd = fd;

	return 0;
}

int SP_DefaultIOChannel :: receive( SP_Session * session )
{
	int ret = 0;
	//evbuffer * buff = getEvBuffer( session->getInBuffer() );
	//ret = evbuffer_read( buff, mFd, 1308 );
	char readBuff[1500]={0};
	ret = recv(mFd, readBuff,1308,0);
	if(ret>0) {
		 mLen += ret;
		LPPACK_HEAD head = (LPPACK_HEAD)readBuff;
		printf("fid:%d pid:%d len:%d ret:%d\n", htonl(head->fid), htons(head->pid), htonl(head->len) - mPackHeadLen, ret);
//		if(head->type.M == -1){
//			printf("package len:%d\n", mLen);
//			mLen = 0;
//		}
		//printf("read ret:%d len:%d id:%d mark:%d\n",ret, htons(head->len), htonl(head->id), head->type.M);
	}
	return ret;
}

int SP_DefaultIOChannel :: write_vec( struct iovec * iovArray, int iovSize )
{
	return writev( mFd, iovArray, iovSize );
}

//---------------------------------------------------------

SP_DefaultIOChannelFactory :: SP_DefaultIOChannelFactory()
{
}

SP_DefaultIOChannelFactory :: ~SP_DefaultIOChannelFactory()
{
}

SP_IOChannel * SP_DefaultIOChannelFactory :: create() const
{
	return new SP_DefaultIOChannel();
}

