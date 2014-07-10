//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D11QueryManager.h"
#include "BsD3D11EventQuery.h"
#include "BsD3D11TimerQuery.h"
#include "BsD3D11OcclusionQuery.h"

namespace BansheeEngine
{
	EventQueryPtr D3D11QueryManager::createEventQuery() const
	{
		EventQueryPtr query = std::shared_ptr<D3D11EventQuery>(bs_new<D3D11EventQuery>(), &QueryManager::deleteEventQuery, StdAlloc<GenAlloc>());  
		mEventQueries.push_back(query.get());

		return query;
	}

	TimerQueryPtr D3D11QueryManager::createTimerQuery() const
	{
		TimerQueryPtr query = std::shared_ptr<D3D11TimerQuery>(bs_new<D3D11TimerQuery>(), &QueryManager::deleteTimerQuery, StdAlloc<GenAlloc>());  
		mTimerQueries.push_back(query.get());

		return query;
	}

	OcclusionQueryPtr D3D11QueryManager::createOcclusionQuery(bool binary) const
	{
		OcclusionQueryPtr query = std::shared_ptr<D3D11OcclusionQuery>(bs_new<D3D11OcclusionQuery>(binary), &QueryManager::deleteOcclusionQuery, StdAlloc<GenAlloc>());
		mOcclusionQueries.push_back(query.get());

		return query;
	}
}