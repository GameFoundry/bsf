//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD3D9QueryManager.h"
#include "BsD3D9EventQuery.h"
#include "BsD3D9TimerQuery.h"
#include "BsD3D9OcclusionQuery.h"

namespace BansheeEngine
{
	SPtr<EventQuery> D3D9QueryManager::createEventQuery() const
	{
		SPtr<EventQuery> query = SPtr<D3D9EventQuery>(bs_new<D3D9EventQuery>(), &QueryManager::deleteEventQuery, StdAlloc<D3D9EventQuery>());
		mEventQueries.push_back(query.get());

		return query;
	}

	SPtr<TimerQuery> D3D9QueryManager::createTimerQuery() const
	{
		SPtr<TimerQuery> query = SPtr<D3D9TimerQuery>(bs_new<D3D9TimerQuery>(), &QueryManager::deleteTimerQuery, StdAlloc<D3D9TimerQuery>());
		mTimerQueries.push_back(query.get());

		return query;
	}

	SPtr<OcclusionQuery> D3D9QueryManager::createOcclusionQuery(bool binary) const
	{
		SPtr<OcclusionQuery> query = SPtr<D3D9OcclusionQuery>(bs_new<D3D9OcclusionQuery>(binary), &QueryManager::deleteOcclusionQuery, StdAlloc<D3D9OcclusionQuery>());
		mOcclusionQueries.push_back(query.get());

		return query;
	}
}