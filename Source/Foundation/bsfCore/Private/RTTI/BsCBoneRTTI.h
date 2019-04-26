//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Components/BsCBone.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_CORE_EXPORT CBoneRTTI : public RTTIType<CBone, Component, CBoneRTTI>
	{
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(mBoneName, 0)
		BS_END_RTTI_MEMBERS
	public:
		const String& getRTTIName() override
		{
			static String name = "CBone";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CBone;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return SceneObject::createEmptyComponent<CBone>();
		}
	};

	/** @} */
	/** @endcond */
}
