#include "CmTimerQuery.h"
#include "CmQueryManager.h"

namespace BansheeEngine
{
	TimerQueryPtr TimerQuery::create()
	{
		return QueryManager::instance().createTimerQuery();
	}
}