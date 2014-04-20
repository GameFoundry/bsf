#pragma once

#include "CmPrerequisites.h"
#include "CmEventQuery.h"
#include "CmModule.h"

namespace BansheeEngine
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
		mutable Vector<EventQuery*>::type mEventQueries;
		mutable Vector<TimerQuery*>::type mTimerQueries;

	protected:
		friend class EventQuery;
		friend class TimerQuery;

		static void deleteEventQuery(EventQuery* query);
		static void deleteTimerQuery(TimerQuery* query);
	};
}