#include "BsGLQueryManager.h"
#include "BsGLEventQuery.h"
#include "BsGLTimerQuery.h"
#include "BsGLOcclusionQuery.h"

namespace BansheeEngine
{
	EventQueryPtr GLQueryManager::createEventQuery() const
	{
		EventQueryPtr query = std::shared_ptr<GLEventQuery>(bs_new<GLEventQuery>(), &QueryManager::deleteEventQuery, StdAlloc<GenAlloc>());  
		mEventQueries.push_back(query.get());

		return query;
	}

	TimerQueryPtr GLQueryManager::createTimerQuery() const
	{
		TimerQueryPtr query = std::shared_ptr<GLTimerQuery>(bs_new<GLTimerQuery>(), &QueryManager::deleteTimerQuery, StdAlloc<GenAlloc>());  
		mTimerQueries.push_back(query.get());

		return query;
	}

	OcclusionQueryPtr GLQueryManager::createOcclusionQuery(bool binary) const
	{
		OcclusionQueryPtr query = std::shared_ptr<GLOcclusionQuery>(bs_new<GLOcclusionQuery>(binary), &QueryManager::deleteOcclusionQuery, StdAlloc<GenAlloc>());
		mOcclusionQueries.push_back(query.get());

		return query;
	}
}