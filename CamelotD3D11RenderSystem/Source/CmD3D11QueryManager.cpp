#include "CmD3D11QueryManager.h"
#include "CmD3D11EventQuery.h"
#include "CmD3D11TimerQuery.h"
#include "CmD3D11OcclusionQuery.h"

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

	OcclusionQueryPtr D3D11QueryManager::createOcclusionQuery(bool binary) const
	{
		OcclusionQueryPtr query = std::shared_ptr<D3D11OcclusionQuery>(cm_new<D3D11OcclusionQuery>(binary), &QueryManager::deleteOcclusionQuery, StdAlloc<GenAlloc>());
		mOcclusionQueries.push_back(query.get());

		return query;
	}
}