//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCJoint.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CJointRTTI : public RTTIType<CJoint, Component, CJointRTTI>
	{
		BS_REFL_MEMBER_NAMED(mBodyA, mBodies[0]);
		BS_REFL_MEMBER_NAMED(mBodyB, mBodies[1]);

		BS_PLAIN_MEMBER_NAMED(mPositionA, mPositions[0]);
		BS_PLAIN_MEMBER_NAMED(mPositionB, mPositions[1]);

		BS_PLAIN_MEMBER_NAMED(mRotationA, mRotations[0]);
		BS_PLAIN_MEMBER_NAMED(mRotationB, mRotations[1]);

		BS_PLAIN_MEMBER_NAMED(mBreakForce, mDesc.breakForce)
		BS_PLAIN_MEMBER_NAMED(mBreakTorque, mDesc.breakTorque)
		BS_PLAIN_MEMBER_NAMED(mEnableCollision, mDesc.enableCollision)

	public:
		CJointRTTI()
		{
			BS_ADD_REFL_FIELD(mBodyA, 0);
			BS_ADD_REFL_FIELD(mBodyB, 1);
			BS_ADD_PLAIN_FIELD(mPositionA, 2);
			BS_ADD_PLAIN_FIELD(mPositionB, 3);
			BS_ADD_PLAIN_FIELD(mRotationA, 4);
			BS_ADD_PLAIN_FIELD(mRotationB, 5);
			BS_ADD_PLAIN_FIELD(mBreakForce, 6);
			BS_ADD_PLAIN_FIELD(mBreakTorque, 7);
			BS_ADD_PLAIN_FIELD(mEnableCollision, 8);
		}

		const String& getRTTIName() override
		{
			static String name = "CJoint";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CJoint;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
			return nullptr;
		}
	};

	/** @} */
	/** @endcond */
}