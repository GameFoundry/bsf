//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	/** @addtogroup D3D9
	 *  @{
	 */

	/**	Handles creation of DirectX 9 queries. */
	class BS_D3D9_EXPORT D3D9QueryManager : public QueryManager
	{
	public:
		/** @copydoc QueryManager::createEventQuery */
		SPtr<EventQuery> createEventQuery() const override;

		/** @copydoc QueryManager::createTimerQuery */
		SPtr<TimerQuery> createTimerQuery() const override;

		/** @copydoc QueryManager::createOcclusionQuery */
		SPtr<OcclusionQuery> createOcclusionQuery(bool binary) const override;
	};

	/** @} */
}