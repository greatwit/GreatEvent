#include "TaskBase.hpp"



	TaskBase::TaskBase( Sid_t sid )
			:mSid(sid)
	{

	}


	TaskBase::~TaskBase() {

	}


	int TaskBase::StartTask() {
		return 0;
	}

	int TaskBase::StopTask() {
		return 0;
	}

	int TaskBase::readBuffer() {
		return 0;
	}

	int TaskBase::writeBuffer() {
		return 0;
	}
