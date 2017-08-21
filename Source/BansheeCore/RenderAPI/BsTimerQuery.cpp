//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "RenderAPI/BsTimerQuery.h"
#include "Managers/BsQueryManager.h"

namespace bs { namespace ct
{
	SPtr<TimerQuery> TimerQuery::create(UINT32 deviceIdx)
	{
		return QueryManager::instance().createTimerQuery(deviceIdx);
	}
}}