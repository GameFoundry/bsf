#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	/**
	 * @brief	Represents a GPU query that measures execution time of GPU operations. The query will measure
	 * 			any GPU operations that take place between its "begin" and "end" calls.
	 * 			
	 * @note	Core thread only.
	 */
	class CM_EXPORT TimerQuery
	{
	public:
		virtual ~TimerQuery() {}

		/**
		 * @brief	Starts the counter. 
		 * 			
		 * @note	Place any commands you want to measure after this call. Call "end" when done.
		 */
		virtual void begin() = 0;

		/**
		 * @brief	Stops the counter.
		 */
		virtual void end() = 0;

		/**
		 * @brief	Check if GPU has processed the query.
		 */
		virtual bool isReady() const = 0;

		/**
		 * @brief	Returns the time it took for the query to execute.
		 *
		 * @return	The time milliseconds.
		 * 			
		 * @note	Only valid after "isReady" returns true.
		 */
		virtual float getTimeMs() = 0;

		boost::signal<void(float)> onTriggered;

		static TimerQueryPtr create();

	protected:
		friend class QueryManager;

		virtual void finalize() = 0;

		bool isActive() const { return mActive; }
		void setActive(bool active) { mActive = active; }

	protected:
		bool mActive;
	};
}