//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"

namespace bs
{
	/** @addtogroup General
	 *  @{
	 */

	/** Timer class used for querying high precision timers. */
	class BS_UTILITY_EXPORT Timer
	{
	public:
		/** Construct the timer and start timing. */
		Timer();

		/** Reset the timer to zero. */
		void reset();

		/** Returns time in milliseconds since timer was initialized or last reset. */
		UINT64 getMilliseconds() const;

		/** Returns time in microseconds since timer was initialized or last reset. */
		UINT64 getMicroseconds() const;

		/**
		 * Returns the time at which the timer was initialized, in milliseconds.
		 *
		 * @return	Time in milliseconds.
		 */
		UINT64 getStartMs() const;

	private:
		std::chrono::high_resolution_clock mHRClock;
		std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
	};

	/** @} */
}
