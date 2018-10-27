

#include "TcpClient.hpp"
#include "IOUtils.hpp"

#include "Session.hpp"

#include "event.h"
#include "basedef.h"
#include "h264.h"

TcpClient :: TcpClient( )
		:mSockId(0)
		,mSession(NULL)
{

}

TcpClient :: ~TcpClient()
{
	if(mSockId > 0) {
		close(mSockId);
		mSockId = 0;
	}

	if(mSession != NULL) {
		//delete mSession;
		//mSession = NULL;
	}
}

int TcpClient :: connect(const char* destIp, unsigned short destPort) {
	int ret = IOUtils::tcpConnect(destIp, destPort, &mSockId, 0);
	GLOGW("connect ret:%d sockid:%d.",ret, mSockId);
	if(ret>=0) {
		mSid.mKey = mSockId;
		mSession = new Session( mSid, VIDEO_SEND_MSG );
	}
	return ret;
}

int TcpClient :: disConnect() {
	if(mSockId > 0) {
		EventArg * eventArg = (EventArg*)mSession->getArg();
		eventArg->getSessionManager()->remove(mSockId);
		event_del(mSession->getReadEvent());
		event_del(mSession->getReadEvent());

		close(mSockId);
		mSockId = 0;
		GLOGW("disConnect sockid:%d.", mSockId);
	}
	return 0;
}

int TcpClient :: registerEvent(const EventArg& evarg) {

	if(mSession!=NULL) {

		evarg.getSessionManager()->get( mSid.mKey, &mSid.mSeq );
		evarg.getSessionManager()->put( mSid.mKey, mSession, &mSid.mSeq );
		mSession->setArg( (void*)&evarg );

		event_set( mSession->getReadEvent(), mSockId, EV_READ, EventCall::onRead, mSession );
		event_set( mSession->getWriteEvent(), mSockId, EV_WRITE, EventCall::onWrite, mSession );
		EventCall::addEvent( mSession, EV_READ, mSockId );
		EventCall::addEvent( mSession, EV_WRITE, mSockId );

		evarg.getSessionManager()->get( mSid.mKey, &mSid.mSeq );
		evarg.getSessionManager()->put( mSid.mKey, mSession, &mSid.mSeq );

	}
	else
		GLOGE("tcpclient registerEvent mSession is NULL.");

	return 0;
}

