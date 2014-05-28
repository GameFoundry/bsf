#include "CmTime.h"
#include "CmTimer.h"

namespace BansheeEngine
{
	const double Time::MICROSEC_TO_SEC = 1.0/1000000.0;

	Time::Time()
		:mAppStartTime(0), mLastFrameTime(0), mFrameDelta(0.0f), mTimeSinceStart(0.0f), mCurrentFrame(0)
	{
		mTimer = cm_new<Timer>();
		mAppStartTime = (UINT64)mTimer->getStartMs();
	}

	Time::~Time()
	{
		cm_delete(mTimer);
	}

	void Time::update()
	{
		unsigned long currentFrameTime = mTimer->getMicroseconds();

		mFrameDelta = (float)((currentFrameTime - mLastFrameTime) * MICROSEC_TO_SEC);
		mTimeSinceStart = (float)(currentFrameTime * MICROSEC_TO_SEC);
		mTimeSinceStartMs = (UINT64)(currentFrameTime / 1000);

		mLastFrameTime = currentFrameTime;

		mCurrentFrame++;
	}

	UINT64 Time::getTimePrecise() const
	{
		return mTimer->getMicroseconds();
	}

	Time& gTime()
	{
		return Time::instance();
	}
}