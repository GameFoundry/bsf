#pragma once

#include "CmPrerequisites.h"
#include "CmEventQuery.h"
#include "CmModule.h"

namespace CamelotFramework
{
	/**
	 * @brief	Handles creation and destruction of GPU queries.
	 * 			
	 * @note	Core thread only.
	 */
	class CM_EXPORT QueryManager : public Module<QueryManager>
	{
	public:
		QueryManager();

		virtual EventQueryPtr createEventQuery() const = 0;
		virtual TimerQueryPtr createTimerQuery() const = 0;

		/**
		 * @note	Internal method, called every frame.
		 */
		void update();

	protected:
		mutable Vector<EventQuery*>::type mEventQueries[2]; // Two buffers so we can ping-pong between them
		mutable Vector<TimerQuery*>::type mTimerQueries[2];
		UINT32 mBufferIdx;

	protected:
		friend class EventQuery;
		friend class TimerQuery;

		static void deleteEventQuery(EventQuery* query);
		static void deleteTimerQuery(TimerQuery* query);
	};
}