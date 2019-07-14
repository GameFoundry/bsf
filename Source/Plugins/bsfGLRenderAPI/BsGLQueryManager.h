//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsGLPrerequisites.h"
#include "Managers/BsQueryManager.h"

namespace bs { namespace ct
{
	/** @addtogroup GL
	 *  @{
	 */

	/**	Handles creation and life of OpenGL queries. */
	class GLQueryManager : public QueryManager
	{
	public:
		/** @copydoc QueryManager::createEventQuery */
		SPtr<EventQuery> createEventQuery(UINT32 deviceIdx = 0) const override;

		/** @copydoc QueryManager::createTimerQuery */
		SPtr<TimerQuery> createTimerQuery(UINT32 deviceIdx = 0) const override;

		/** @copydoc QueryManager::createOcclusionQuery */
		SPtr<OcclusionQuery> createOcclusionQuery(bool binary, UINT32 deviceIdx = 0) const override;
	};

	/** @} */
}}
