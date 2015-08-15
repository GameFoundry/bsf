#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of DirectX 11 queries.
	 */
	class BS_D3D11_EXPORT D3D11QueryManager : public QueryManager
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