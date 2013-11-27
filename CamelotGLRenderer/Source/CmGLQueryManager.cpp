#include "CmGLQueryManager.h"
#include "CmGLEventQuery.h"
#include "CmGLTimerQuery.h"

namespace CamelotFramework
{
	EventQueryPtr GLQueryManager::createEventQuery() const
	{
		EventQueryPtr query = std::shared_ptr<GLEventQuery>(cm_new<GLEventQuery>(), &QueryManager::deleteEventQuery, StdAlloc<GenAlloc>());  
		mEventQueries.push_back(query.get());

		return query;
	}

	TimerQueryPtr GLQueryManager::createTimerQuery() const
	{
		TimerQueryPtr query = std::shared_ptr<GLTimerQuery>(cm_new<GLTimerQuery>(), &QueryManager::deleteTimerQuery, StdAlloc<GenAlloc>());  
		mTimerQueries.push_back(query.get());

		return query;
	}
}