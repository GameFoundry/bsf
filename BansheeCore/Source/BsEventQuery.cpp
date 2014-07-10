//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsEventQuery.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	EventQueryPtr EventQuery::create()
	{
		return QueryManager::instance().createEventQuery();
	}
}