//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCHingeJoint.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CHingeJointRTTI : public RTTIType<CHingeJoint, CJoint, CHingeJointRTTI>
	{
		BS_PLAIN_MEMBER_NAMED(mFlag, mDesc.flag)
		BS_PLAIN_MEMBER_NAMED(mDriveSpeed, mDesc.drive.speed)
		BS_PLAIN_MEMBER_NAMED(mDriveForceLimit, mDesc.drive.forceLimit)
		BS_PLAIN_MEMBER_NAMED(mDriveGearRatio, mDesc.drive.gearRatio)
		BS_PLAIN_MEMBER_NAMED(mDriveFreeSpin, mDesc.drive.freeSpin)
		BS_PLAIN_MEMBER_NAMED(mLimitLower, mDesc.limit.lower)
		BS_PLAIN_MEMBER_NAMED(mLimitUpper, mDesc.limit.upper)
		BS_PLAIN_MEMBER_NAMED(mLimitContactDist, mDesc.limit.contactDist)
		BS_PLAIN_MEMBER_NAMED(mLimitRestitution, mDesc.limit.restitution)
		BS_PLAIN_MEMBER_NAMED(mSpringDamping, mDesc.limit.spring.damping)
		BS_PLAIN_MEMBER_NAMED(mSpringStiffness, mDesc.limit.spring.stiffness)

	public:
		CHingeJointRTTI()
		{
			BS_ADD_PLAIN_FIELD(mFlag, 0);
			BS_ADD_PLAIN_FIELD(mDriveSpeed, 1);
			BS_ADD_PLAIN_FIELD(mDriveForceLimit, 2);
			BS_ADD_PLAIN_FIELD(mDriveGearRatio, 3);
			BS_ADD_PLAIN_FIELD(mDriveFreeSpin, 4);
			BS_ADD_PLAIN_FIELD(mLimitLower, 5);
			BS_ADD_PLAIN_FIELD(mLimitUpper, 6);
			BS_ADD_PLAIN_FIELD(mLimitContactDist, 7);
			BS_ADD_PLAIN_FIELD(mLimitRestitution, 8);
			BS_ADD_PLAIN_FIELD(mSpringDamping, 9);
			BS_ADD_PLAIN_FIELD(mSpringStiffness, 10);
		}

		const String& getRTTIName() override
		{
			static String name = "CHingeJoint";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CHingeJoint;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CHingeJoint>();
		}
	};

	/** @} */
	/** @endcond */
}