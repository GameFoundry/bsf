//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Components/BsCLightProbeVolume.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_CORE_EXPORT CLightProbeVolumeRTTI : public RTTIType <CLightProbeVolume, Component, CLightProbeVolumeRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_REFLPTR(mInternal, 0)
		BS_END_RTTI_MEMBERS

	public:
		CLightProbeVolumeRTTI() = default;

		const String& getRTTIName() override
		{
			static String name = "CLightProbeVolume";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CLightProbeVolume;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CLightProbeVolume>();
		}
	};

	/** @} */
	/** @endcond */
}
