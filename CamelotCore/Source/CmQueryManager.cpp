#include "CmQueryManager.h"
#include "CmEventQuery.h"
#include "CmTimerQuery.h"
#include "CmOcclusionQuery.h"

namespace BansheeEngine
{
	QueryManager::QueryManager()
	{

	}

	void QueryManager::_update()
	{
		for(auto& query : mEventQueries)
		{
			if(query->isActive() && query->isReady())
			{
				query->onTriggered();
				query->setActive(false);
			}
		}

		for(auto& query : mTimerQueries)
		{
			if(query->isActive() && query->isReady())
			{
				query->onTriggered(query->getTimeMs());
				query->setActive(false);
			}
		}

		for (auto& query : mOcclusionQueries)
		{
			if (query->isActive() && query->isReady())
			{
				query->onComplete(query->getNumSamples());
				query->setActive(false);
			}
		}
	}

	void QueryManager::deleteEventQuery(EventQuery* query)
	{
		auto iterFind = std::find(instance().mEventQueries.begin(), instance().mEventQueries.end(), query);

		if(iterFind != instance().mEventQueries.end())
			instance().mEventQueries.erase(iterFind);

		cm_delete(query);
	}

	void QueryManager::deleteTimerQuery(TimerQuery* query)
	{
		auto iterFind = std::find(instance().mTimerQueries.begin(), instance().mTimerQueries.end(), query);

		if(iterFind != instance().mTimerQueries.end())
			instance().mTimerQueries.erase(iterFind);

		cm_delete(query);
	}

	void QueryManager::deleteOcclusionQuery(OcclusionQuery* query)
	{
		auto iterFind = std::find(instance().mOcclusionQueries.begin(), instance().mOcclusionQueries.end(), query);

		if (iterFind != instance().mOcclusionQueries.end())
			instance().mOcclusionQueries.erase(iterFind);

		cm_delete(query);
	}
}