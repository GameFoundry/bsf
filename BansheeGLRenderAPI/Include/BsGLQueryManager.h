#pragma once

#include "BsGLPrerequisites.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation and life of OpenGL queries.
	 */
	class BS_RSGL_EXPORT GLQueryManager : public QueryManager
	{
	public:
		/**
		 * @copydoc	QueryManager::createEventQuery
		 */
		EventQueryPtr createEventQuery() const;

		/**
		 * @copydoc	QueryManager::createTimerQuery
		 */
		TimerQueryPtr createTimerQuery() const;

		/**
		 * @copydoc	QueryManager::createOcclusionQuery
		 */
		OcclusionQueryPtr createOcclusionQuery(bool binary) const;
	};
}