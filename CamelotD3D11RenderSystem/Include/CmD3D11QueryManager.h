#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmQueryManager.h"

namespace BansheeEngine
{
	class CM_D3D11_EXPORT D3D11QueryManager : public QueryManager
	{
	public:
		EventQueryPtr createEventQuery() const;
		TimerQueryPtr createTimerQuery() const;
	};
}