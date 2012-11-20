#include "CmTime.h"
#include "CmTimer.h"

namespace CamelotEngine
{
	const double Time::MICROSEC_TO_SEC = 1.0/1000000.0;

	Time::Time()
		:mAppStartTime(0), mLastFrameTime(0), mFrameDelta(0.0f), mTimeSinceStart(0.0f), mCurrentFrame(0)
	{
		mTimer = new Timer();
		mAppStartTime = mTimer->getMicroseconds();
	}

	Time::~Time()
	{
		delete mTimer;
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
		// TODO Low priority - Timer internally calls high performance OS specific methods. We can go a step further and use CPU specific instructions, which would
		// (likely) give even more precise measurements in cycles. (RDTSC instruction - although that might not be valid with todays variable CPU clocks)
		return mTimer->getMicroseconds();
	}

	Time& gTime()
	{
		return Time::instance();
	}
}