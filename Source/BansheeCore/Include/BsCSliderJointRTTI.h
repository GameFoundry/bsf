//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCSliderJoint.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CSliderJointRTTI : public RTTIType<CSliderJoint, CJoint, CSliderJointRTTI>
	{
		BS_PLAIN_MEMBER(mFlag)
		BS_PLAIN_MEMBER_NAMED(mLimitLower, mLimit.lower)
		BS_PLAIN_MEMBER_NAMED(mLimitUpper, mLimit.upper)
		BS_PLAIN_MEMBER_NAMED(mLimitContactDist, mLimit.contactDist)
		BS_PLAIN_MEMBER_NAMED(mLimitRestitution, mLimit.restitution)
		BS_PLAIN_MEMBER_NAMED(mSpringDamping, mLimit.spring.damping)
		BS_PLAIN_MEMBER_NAMED(mSpringStiffness, mLimit.spring.stiffness)

	public:
		CSliderJointRTTI()
		{
			BS_ADD_PLAIN_FIELD(mFlag, 0);
			BS_ADD_PLAIN_FIELD(mLimitLower, 1);
			BS_ADD_PLAIN_FIELD(mLimitUpper, 2);
			BS_ADD_PLAIN_FIELD(mLimitContactDist, 3);
			BS_ADD_PLAIN_FIELD(mLimitRestitution, 4);
			BS_ADD_PLAIN_FIELD(mSpringDamping, 5);
			BS_ADD_PLAIN_FIELD(mSpringStiffness, 6);
		}

		const String& getRTTIName() override
		{
			static String name = "CSliderJoint";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CSliderJoint;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CSliderJoint>();
		}
	};

	/** @} */
	/** @endcond */
}