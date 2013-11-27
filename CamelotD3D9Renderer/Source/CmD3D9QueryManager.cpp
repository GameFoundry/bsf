#include "CmD3D9QueryManager.h"
#include "CmD3D9EventQuery.h"
#include "CmD3D9TimerQuery.h"

namespace CamelotFramework
{
	EventQueryPtr D3D9QueryManager::createEventQuery() const
	{
		EventQueryPtr query = std::shared_ptr<D3D9EventQuery>(cm_new<D3D9EventQuery>(), &QueryManager::deleteEventQuery, StdAlloc<GenAlloc>());  
		mEventQueries.push_back(query.get());

		return query;
	}

	TimerQueryPtr D3D9QueryManager::createTimerQuery() const
	{
		TimerQueryPtr query = std::shared_ptr<D3D9TimerQuery>(cm_new<D3D9TimerQuery>(), &QueryManager::deleteTimerQuery, StdAlloc<GenAlloc>());  
		mTimerQueries.push_back(query.get());

		return query;
	}
}