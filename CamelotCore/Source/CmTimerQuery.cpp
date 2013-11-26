#include "CmTimerQuery.h"
#include "CmQueryManager.h"

namespace CamelotFramework
{
	TimerQueryPtr TimerQuery::create()
	{
		return QueryManager::instance().createTimerQuery();
	}
}