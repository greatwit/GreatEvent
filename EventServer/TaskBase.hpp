#ifndef __TaskBase_hpp__
#define __TaskBase_hpp__



#include <stdint.h>


class Session;

typedef struct tagSid {
	uint16_t mKey;//sockid
	uint16_t mSeq;

	enum {
		eTimerKey = 0,
		eTimerSeq = 65535
	};
} Sid_t;

class TaskBase {
public:
	TaskBase( Sid_t sid );
	TaskBase( Sid_t &sid, char*filename);
	TaskBase(  Session*sess, Sid_t&sid, char*filename);
	virtual ~TaskBase();
	virtual int StartTask();
	virtual int StopTask();
	virtual int setHeartCount();
	virtual int readBuffer();
	virtual int writeBuffer();

protected:
	Sid_t mSid;
	int   mHeartCount;
};


#endif


