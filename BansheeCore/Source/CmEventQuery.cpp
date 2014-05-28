#include "CmEventQuery.h"
#include "CmQueryManager.h"

namespace BansheeEngine
{
	EventQueryPtr EventQuery::create()
	{
		return QueryManager::instance().createEventQuery();
	}
}