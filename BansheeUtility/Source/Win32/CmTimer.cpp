#include "CmTimer.h"
#include "CmBitwise.h"

namespace BansheeEngine
{
	Timer::Timer()
	{
		reset();
	}

	Timer::~Timer()
	{
	}

	void Timer::reset()
	{
		QueryPerformanceFrequency(&mFrequency);
		QueryPerformanceCounter(&mStartTime);

		mZeroClock = clock();
	}

	unsigned long Timer::getMilliseconds()
	{
		LARGE_INTEGER curTime;
		QueryPerformanceCounter(&curTime);

		LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;
    
		// Scale by 1000 for milliseconds
		unsigned long newTicks = (unsigned long) (1000 * newTime / mFrequency.QuadPart);

		return newTicks;
	}

	unsigned long Timer::getStartMs() const
	{
		unsigned long newTicks = (unsigned long) (1000 * mStartTime.QuadPart / mFrequency.QuadPart);

		return newTicks;
	}

	unsigned long Timer::getMicroseconds()
	{
		LARGE_INTEGER curTime;
		QueryPerformanceCounter(&curTime);

		LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;
    
		// Scale by 1000000 for microseconds
		unsigned long newMicro = (unsigned long) (1000000 * newTime / mFrequency.QuadPart);

		return newMicro;
	}

	unsigned long Timer::getMillisecondsCPU()
	{
		clock_t newClock = clock();
		return (unsigned long)((float)(newClock - mZeroClock) / ((float)CLOCKS_PER_SEC / 1000.0f));
	}


	unsigned long Timer::getMicrosecondsCPU()
	{
		clock_t newClock = clock();
		return (unsigned long)((float)(newClock - mZeroClock) / ((float)CLOCKS_PER_SEC / 1000000.0f));
	}
}