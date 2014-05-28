#include "CmOcclusionQuery.h"
#include "CmQueryManager.h"

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