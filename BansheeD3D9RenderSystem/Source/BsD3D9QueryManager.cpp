#include "BsD3D9QueryManager.h"
#include "BsD3D9EventQuery.h"
#include "BsD3D9TimerQuery.h"
#include "BsD3D9OcclusionQuery.h"

namespace BansheeEngine
{
	EventQueryPtr D3D9QueryManager::createEventQuery() const
	{
		EventQueryPtr query = std::shared_ptr<D3D9EventQuery>(bs_new<D3D9EventQuery>(), &QueryManager::deleteEventQuery, StdAlloc<GenAlloc>());  
		mEventQueries.push_back(query.get());

		return query;
	}

	TimerQueryPtr D3D9QueryManager::createTimerQuery() const
	{
		TimerQueryPtr query = std::shared_ptr<D3D9TimerQuery>(bs_new<D3D9TimerQuery>(), &QueryManager::deleteTimerQuery, StdAlloc<GenAlloc>());  
		mTimerQueries.push_back(query.get());

		return query;
	}

	OcclusionQueryPtr D3D9QueryManager::createOcclusionQuery(bool binary) const
	{
		OcclusionQueryPtr query = std::shared_ptr<D3D9OcclusionQuery>(bs_new<D3D9OcclusionQuery>(binary), &QueryManager::deleteOcclusionQuery, StdAlloc<GenAlloc>());
		mOcclusionQueries.push_back(query.get());

		return query;
	}
}