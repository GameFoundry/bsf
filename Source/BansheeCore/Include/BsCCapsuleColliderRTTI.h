//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCCapsuleCollider.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CCapsuleColliderRTTI : public RTTIType<CCapsuleCollider, CCollider, CCapsuleColliderRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mNormal)
		BS_PLAIN_MEMBER(mRadius)
		BS_PLAIN_MEMBER(mHalfHeight)
	public:
		CCapsuleColliderRTTI()
		{
			BS_ADD_PLAIN_FIELD(mNormal, 0);
			BS_ADD_PLAIN_FIELD(mRadius, 1);
			BS_ADD_PLAIN_FIELD(mHalfHeight, 2);
		}

		const String& getRTTIName() override
		{
			static String name = "CCapsuleCollider";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CCapsuleCollider;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CCapsuleCollider>();
		}
	};

	/** @} */
	/** @endcond */
}