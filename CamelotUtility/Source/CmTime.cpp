#include "CmTime.h"
#include "CmTimer.h"

namespace CamelotEngine
{
	const double Time::MICROSEC_TO_SEC = 1.0/1000000.0;

	Time::Time()
		:mAppStartTime(0), mLastFrameTime(0), mFrameDelta(0.0f), mTimeSinceStart(0.0f), mCurrentFrame(0)
	{
		mTimer = new Timer();
	}

	Time::~Time()
	{
		delete mTimer;
	}

	void Time::init()
	{
		mAppStartTime = mTimer->getMicroseconds();
	}

	void Time::update()
	{
		unsigned long currentFrameTime = mTimer->getMicroseconds();

		mFrameDelta = (float)((currentFrameTime - mLastFrameTime) * MICROSEC_TO_SEC);
		mTimeSinceStart = (float)(currentFrameTime * MICROSEC_TO_SEC);

		mLastFrameTime = currentFrameTime;

		mCurrentFrame++;
	}

	Time& gTime()
	{
		return Time::instance();
	}
}