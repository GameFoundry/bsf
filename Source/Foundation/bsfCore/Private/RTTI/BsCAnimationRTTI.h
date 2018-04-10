//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Components/BsCAnimation.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_CORE_EXPORT CAnimationRTTI : public RTTIType<CAnimation, Component, CAnimationRTTI>
	{
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_REFL(mDefaultClip, 0)
			BS_RTTI_MEMBER_PLAIN(mWrapMode, 1)
			BS_RTTI_MEMBER_PLAIN(mSpeed, 2)
			BS_RTTI_MEMBER_PLAIN(mEnableCull, 3)
			BS_RTTI_MEMBER_PLAIN(mUseBounds, 4)
			BS_RTTI_MEMBER_PLAIN(mBounds, 5)
		BS_END_RTTI_MEMBERS
	public:
		CAnimationRTTI() = default;

		const String& getRTTIName() override
		{
			static String name = "CAnimation";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CAnimation;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CAnimation>();
		}
	};

	/** @} */
	/** @endcond */
}
