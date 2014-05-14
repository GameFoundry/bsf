#pragma once

#include "CmGLPrerequisites.h"
#include "CmQueryManager.h"

namespace BansheeEngine
{
	class CM_RSGL_EXPORT GLQueryManager : public QueryManager
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