//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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