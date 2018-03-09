//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "GUI/BsProfilerOverlay.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_EXPORT ProfilerOverlayRTTI : public RTTIType <CProfilerOverlay, Component, ProfilerOverlayRTTI>
	{
	private:

	public:
		ProfilerOverlayRTTI()
		{ }

		const String& getRTTIName() override
		{
			static String name = "ProfilerOverlay";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ProfilerOverlay;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CProfilerOverlay>();
		}
	};

	/** @} */
	/** @endcond */
}