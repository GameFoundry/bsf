//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Managers/BsQueryManager.h"
#include "RenderAPI/BsEventQuery.h"
#include "RenderAPI/BsTimerQuery.h"
#include "RenderAPI/BsOcclusionQuery.h"
#include "Profiling/BsProfilerCPU.h"

namespace bs { namespace ct
{
	QueryManager::~QueryManager()
	{
		// Trigger all remaining queries, whether they completed or not

		for (auto& query : mEventQueries)
		{
			if (query->isActive())
				query->onTriggered();
		}

		for (auto& query : mTimerQueries)
		{
			if (query->isActive())
				query->onTriggered(query->getTimeMs());
		}

		for (auto& query : mOcclusionQueries)
		{
			if (query->isActive())
				query->onComplete(query->getNumSamples());
		}

		processDeletedQueue();
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

		processDeletedQueue();
	}

	void QueryManager::deleteEventQuery(EventQuery* query)
	{
		instance().mDeletedEventQueries.push_back(query);
	}

	void QueryManager::deleteTimerQuery(TimerQuery* query)
	{
		instance().mDeletedTimerQueries.push_back(query);
	}

	void QueryManager::deleteOcclusionQuery(OcclusionQuery* query)
	{
		instance().mDeletedOcclusionQueries.push_back(query);
	}

	void QueryManager::processDeletedQueue()
	{
		for (auto& query : mDeletedEventQueries)
		{
			auto iterFind = std::find(mEventQueries.begin(), mEventQueries.end(), query);

			if (iterFind != mEventQueries.end())
				mEventQueries.erase(iterFind);

			bs_delete(query);
		}

		mDeletedEventQueries.clear();

		for (auto& query : mDeletedTimerQueries)
		{
			auto iterFind = std::find(mTimerQueries.begin(), mTimerQueries.end(), query);

			if (iterFind != mTimerQueries.end())
				mTimerQueries.erase(iterFind);

			bs_delete(query);
		}

		mDeletedTimerQueries.clear();

		for (auto& query : mDeletedOcclusionQueries)
		{
			auto iterFind = std::find(mOcclusionQueries.begin(), mOcclusionQueries.end(), query);

			if (iterFind != mOcclusionQueries.end())
				mOcclusionQueries.erase(iterFind);

			bs_delete(query);
		}

		mDeletedOcclusionQueries.clear();
	}
}}
