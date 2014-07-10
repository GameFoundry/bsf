//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsTimerQuery.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	TimerQueryPtr TimerQuery::create()
	{
		return QueryManager::instance().createTimerQuery();
	}
}