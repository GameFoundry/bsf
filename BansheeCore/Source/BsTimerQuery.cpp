#include "BsTimerQuery.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	TimerQueryPtr TimerQuery::create()
	{
		return QueryManager::instance().createTimerQuery();
	}
}