//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsTimer.h"
#include "BsBitwise.h"

#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // Required to stop windows.h messing up std::min
#endif
#include "windows.h"

namespace BansheeEngine
{
	struct Timer::Data
	{
		clock_t zeroClock;

		LARGE_INTEGER startTime;
		LARGE_INTEGER frequency;
	};

	Timer::Timer()
	{
		m = bs_new<Data>();
		reset();
	}

	Timer::~Timer()
	{
		bs_delete(m);
	}

	void Timer::reset()
	{
		QueryPerformanceFrequency(&m->frequency);
		QueryPerformanceCounter(&m->startTime);

		m->zeroClock = clock();
	}

	unsigned long Timer::getMilliseconds()
	{
		LARGE_INTEGER curTime;
		QueryPerformanceCounter(&curTime);

		LONGLONG newTime = curTime.QuadPart - m->startTime.QuadPart;
    
		// Scale by 1000 for milliseconds
		unsigned long newTicks = (unsigned long)(1000 * newTime / m->frequency.QuadPart);

		return newTicks;
	}

	unsigned long Timer::getStartMs() const
	{
		unsigned long newTicks = (unsigned long)(1000 * m->startTime.QuadPart / m->frequency.QuadPart);

		return newTicks;
	}

	unsigned long Timer::getMicroseconds()
	{
		LARGE_INTEGER curTime;
		QueryPerformanceCounter(&curTime);

		LONGLONG newTime = curTime.QuadPart - m->startTime.QuadPart;
    
		// Scale by 1000000 for microseconds
		unsigned long newMicro = (unsigned long)(1000000 * newTime / m->frequency.QuadPart);

		return newMicro;
	}

	unsigned long Timer::getMillisecondsCPU()
	{
		clock_t newClock = clock();
		return (unsigned long)((float)(newClock - m->zeroClock) / ((float)CLOCKS_PER_SEC / 1000.0f));
	}


	unsigned long Timer::getMicrosecondsCPU()
	{
		clock_t newClock = clock();
		return (unsigned long)((float)(newClock - m->zeroClock) / ((float)CLOCKS_PER_SEC / 1000000.0f));
	}
}