#pragma once

#include "CmGLPrerequisites.h"
#include "CmQueryManager.h"

namespace BansheeEngine
{
	class CM_RSGL_EXPORT GLQueryManager : public QueryManager
	{
	public:
		EventQueryPtr createEventQuery() const;
		TimerQueryPtr createTimerQuery() const;
	};
}