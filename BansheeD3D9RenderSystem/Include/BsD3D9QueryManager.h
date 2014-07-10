//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of DirectX 9 queries.
	 */
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