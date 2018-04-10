//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Components/BsCDistanceJoint.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CDistanceJointRTTI : public RTTIType<CDistanceJoint, CJoint, CDistanceJointRTTI>
	{
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN_NAMED(mFlag, mDesc.flag, 0)
			BS_RTTI_MEMBER_PLAIN_NAMED(mMinDistance, mDesc.minDistance, 1)
			BS_RTTI_MEMBER_PLAIN_NAMED(mMaxDistance, mDesc.maxDistance, 2)
			BS_RTTI_MEMBER_PLAIN_NAMED(mTolerance, mDesc.tolerance, 3)
			BS_RTTI_MEMBER_PLAIN_NAMED(mSpringDamping, mDesc.spring.damping, 4)
			BS_RTTI_MEMBER_PLAIN_NAMED(mSpringStiffness, mDesc.spring.stiffness, 5)
		BS_END_RTTI_MEMBERS

	public:
		CDistanceJointRTTI() = default;

		const String& getRTTIName() override
		{
			static String name = "CDistanceJoint";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CDistanceJoint;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CDistanceJoint>();
		}
	};

	/** @} */
	/** @endcond */
}
