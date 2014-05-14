#include "CmD3D9QueryManager.h"
#include "CmD3D9EventQuery.h"
#include "CmD3D9TimerQuery.h"
#include "CmD3D9OcclusionQuery.h"

namespace BansheeEngine
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

	OcclusionQueryPtr D3D9QueryManager::createOcclusionQuery(bool binary) const
	{
		OcclusionQueryPtr query = std::shared_ptr<D3D9OcclusionQuery>(cm_new<D3D9OcclusionQuery>(binary), &QueryManager::deleteOcclusionQuery, StdAlloc<GenAlloc>());
		mOcclusionQueries.push_back(query.get());

		return query;
	}
}