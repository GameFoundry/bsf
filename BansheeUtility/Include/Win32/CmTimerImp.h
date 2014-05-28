#pragma once

#include "../CmPrerequisitesUtil.h"

#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // Required to stop windows.h messing up std::min
#endif
#include "windows.h"

namespace BansheeEngine
{
	/**
	 * @brief	Timer class used for querying high precision timers.
	 */
	class BS_UTILITY_EXPORT Timer
    {
    public:
		/**
		 * @brief	Construct the timer and start timing.
		 */
		Timer();
		~Timer();

		/**
		 * @brief	Reset the timer to zero.
		 */
		void reset();

		/**
		 * @brief	Returns time in milliseconds since timer was initialized or
		 * 			last reset.
		 */
		unsigned long getMilliseconds();

		/**
		 * @brief	Returns time in microseconds since timer was initialized or
		 * 			last reset.
		 */
		unsigned long getMicroseconds();

		/**
		 * @brief	Returns time in milliseconds since timer was initialized or
		 * 			last reset. Only CPU timer measured.
		 */
		unsigned long getMillisecondsCPU();

		/**
		 * @brief	Returns time in microseconds since timer was initialized or
		 * 			last reset. Only CPU timer measured.
		 */
		unsigned long getMicrosecondsCPU();

		/**
		 * @brief	Returns the time at which the timer was initialized, in milliseconds.
		 *
		 * @return	Time in milliseconds.
		 */
		unsigned long getStartMs() const;

	private:
		clock_t mZeroClock;

		LARGE_INTEGER mStartTime;
		LARGE_INTEGER mFrequency;
    };
}