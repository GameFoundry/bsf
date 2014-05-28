#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmQueryManager.h"

namespace BansheeEngine
{
	class BS_D3D9_EXPORT D3D9QueryManager : public QueryManager
	{
	public:
		/**
		 * @copydoc		QueryManager::createEventQuery
		 */
		EventQueryPtr createEventQuery() const;

		/**
		 * @copydoc		QueryManager::createTimerQuery
		 */
		TimerQueryPtr createTimerQuery() const;

		/**
		 * @copydoc		QueryManager::createOcclusionQuery
		 */
		OcclusionQueryPtr createOcclusionQuery(bool binary) const;
	};
}