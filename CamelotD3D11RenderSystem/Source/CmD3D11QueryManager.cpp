#include "CmD3D11QueryManager.h"
#include "CmD3D11EventQuery.h"
#include "CmD3D11TimerQuery.h"

namespace BansheeEngine
{
	EventQueryPtr D3D11QueryManager::createEventQuery() const
	{
		EventQueryPtr query = std::shared_ptr<D3D11EventQuery>(cm_new<D3D11EventQuery>(), &QueryManager::deleteEventQuery, StdAlloc<GenAlloc>());  
		mEventQueries.push_back(query.get());

		return query;
	}

	TimerQueryPtr D3D11QueryManager::createTimerQuery() const
	{
		TimerQueryPtr query = std::shared_ptr<D3D11TimerQuery>(cm_new<D3D11TimerQuery>(), &QueryManager::deleteTimerQuery, StdAlloc<GenAlloc>());  
		mTimerQueries.push_back(query.get());

		return query;
	}
}