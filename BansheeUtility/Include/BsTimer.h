#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Timer class used for querying high precision timers.
	 *
	 * @note	Not thread safe.
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
		struct Data;
		Data* m;
    };
}
