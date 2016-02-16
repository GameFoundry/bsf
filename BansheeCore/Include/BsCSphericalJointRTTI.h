//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCSphericalJoint.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CSphericalJointRTTI : public RTTIType<CSphericalJoint, CJoint, CSphericalJointRTTI>
	{
		BS_PLAIN_MEMBER(mFlag)
		BS_PLAIN_MEMBER_NAMED(mYLimitAngle, mLimit.yLimitAngle)
		BS_PLAIN_MEMBER_NAMED(mZLimitAngle, mLimit.zLimitAngle)
		BS_PLAIN_MEMBER_NAMED(mLimitContactDist, mLimit.contactDist)
		BS_PLAIN_MEMBER_NAMED(mLimitRestitution, mLimit.restitution)
		BS_PLAIN_MEMBER_NAMED(mSpringDamping, mLimit.spring.damping)
		BS_PLAIN_MEMBER_NAMED(mSpringStiffness, mLimit.spring.stiffness)

	public:
		CSphericalJointRTTI()
		{
			BS_ADD_PLAIN_FIELD(mFlag, 0);
			BS_ADD_PLAIN_FIELD(mYLimitAngle, 1);
			BS_ADD_PLAIN_FIELD(mZLimitAngle, 2);
			BS_ADD_PLAIN_FIELD(mLimitContactDist, 3);
			BS_ADD_PLAIN_FIELD(mLimitRestitution, 4);
			BS_ADD_PLAIN_FIELD(mSpringDamping, 5);
			BS_ADD_PLAIN_FIELD(mSpringStiffness, 6);
		}

		const String& getRTTIName() override
		{
			static String name = "CSphericalJoint";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CSphericalJoint;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CSphericalJoint>();
		}
	};

	/** @} */
	/** @endcond */
}