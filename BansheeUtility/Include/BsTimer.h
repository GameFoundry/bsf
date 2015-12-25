#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/** @addtogroup General
	 *  @{
	 */

	/**
	 * Timer class used for querying high precision timers.
	 *
	 * @note	Not thread safe.
	 */
	class BS_UTILITY_EXPORT Timer
    {
    public:
		/** Construct the timer and start timing. */
		Timer();
		~Timer();

		/** Reset the timer to zero. */
		void reset();

		/** Returns time in milliseconds since timer was initialized or last reset. */
		unsigned long getMilliseconds();

		/** Returns time in microseconds since timer was initialized or last reset. */
		unsigned long getMicroseconds();

		/** Returns time in milliseconds since timer was initialized or last reset. Only CPU timer measured. */
		unsigned long getMillisecondsCPU();

		/** Returns time in microseconds since timer was initialized or last reset. Only CPU timer measured. */
		unsigned long getMicrosecondsCPU();

		/**
		 * Returns the time at which the timer was initialized, in milliseconds.
		 *
		 * @return	Time in milliseconds.
		 */
		unsigned long getStartMs() const;

	private:
		struct Data;
		Data* m;
    };

	/** @} */
}
