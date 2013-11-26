#include "CmQueryManager.h"
#include "CmEventQuery.h"
#include "CmTimerQuery.h"

namespace CamelotFramework
{
	QueryManager::QueryManager()
		:mBufferIdx(0)
	{

	}

	void QueryManager::update()
	{
		UINT32 otherBufferIdx = (mBufferIdx + 1) % 2;
		
		for(auto& query : mEventQueries[mBufferIdx])
		{
			if(query->isReady())
				query->onTriggered();
			else
				mEventQueries[otherBufferIdx].push_back(query);
		}

		for(auto& query : mTimerQueries[mBufferIdx])
		{
			if(query->isReady())
				query->onTriggered(query->getTimeMs());
			else
				mTimerQueries[otherBufferIdx].push_back(query);
		}

		mEventQueries[mBufferIdx].clear();
		mTimerQueries[mBufferIdx].clear();

		mBufferIdx = otherBufferIdx;
	}

	void QueryManager::deleteEventQuery(EventQuery* query)
	{
		UINT32 bufferIdx = instance().mBufferIdx;

		auto iterFind = std::find(instance().mEventQueries[bufferIdx].begin(), instance().mEventQueries[bufferIdx].end(), query);

		if(iterFind != instance().mEventQueries[bufferIdx].end())
			instance().mEventQueries[bufferIdx].erase(iterFind);

		cm_delete(query);
	}

	void QueryManager::deleteTimerQuery(TimerQuery* query)
	{
		UINT32 bufferIdx = instance().mBufferIdx;

		auto iterFind = std::find(instance().mTimerQueries[bufferIdx].begin(), instance().mTimerQueries[bufferIdx].end(), query);

		if(iterFind != instance().mTimerQueries[bufferIdx].end())
			instance().mTimerQueries[bufferIdx].erase(iterFind);

		cm_delete(query);
	}
}