#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmQueryManager.h"

namespace CamelotFramework
{
	class CM_D3D9_EXPORT D3D9QueryManager : public QueryManager
	{
	public:
		EventQueryPtr createEventQuery() const;
		TimerQueryPtr createTimerQuery() const;
	};
}