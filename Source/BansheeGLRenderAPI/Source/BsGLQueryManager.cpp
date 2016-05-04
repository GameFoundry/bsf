//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsGLQueryManager.h"
#include "BsGLEventQuery.h"
#include "BsGLTimerQuery.h"
#include "BsGLOcclusionQuery.h"

namespace BansheeEngine
{
	SPtr<EventQuery> GLQueryManager::createEventQuery() const
	{
		SPtr<EventQuery> query = SPtr<GLEventQuery>(bs_new<GLEventQuery>(), &QueryManager::deleteEventQuery, StdAlloc<GLEventQuery>());
		mEventQueries.push_back(query.get());

		return query;
	}

	SPtr<TimerQuery> GLQueryManager::createTimerQuery() const
	{
		SPtr<TimerQuery> query = SPtr<GLTimerQuery>(bs_new<GLTimerQuery>(), &QueryManager::deleteTimerQuery, StdAlloc<GLTimerQuery>());
		mTimerQueries.push_back(query.get());

		return query;
	}

	SPtr<OcclusionQuery> GLQueryManager::createOcclusionQuery(bool binary) const
	{
		SPtr<OcclusionQuery> query = SPtr<GLOcclusionQuery>(bs_new<GLOcclusionQuery>(binary), &QueryManager::deleteOcclusionQuery, StdAlloc<GLOcclusionQuery>());
		mOcclusionQueries.push_back(query.get());

		return query;
	}
}