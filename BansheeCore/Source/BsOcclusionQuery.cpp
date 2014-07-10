//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsOcclusionQuery.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	OcclusionQuery::OcclusionQuery(bool binary)
		:mActive(false), mBinary(binary)
	{ }

	OcclusionQueryPtr OcclusionQuery::create(bool binary)
	{
		return QueryManager::instance().createOcclusionQuery(binary);
	}
}