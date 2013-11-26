#include "CmEventQuery.h"
#include "CmQueryManager.h"

namespace CamelotFramework
{
	EventQueryPtr EventQuery::create()
	{
		return QueryManager::instance().createEventQuery();
	}
}