#include "BsEventQuery.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	EventQueryPtr EventQuery::create()
	{
		return QueryManager::instance().createEventQuery();
	}
}