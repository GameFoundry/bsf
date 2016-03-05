//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCD6Joint.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	float stiffness = 0.0f;
	float damping = 0.0f;
	float forceLimit = FLT_MAX;
	bool acceleration = false;

	class BS_CORE_EXPORT CD6JointRTTI : public RTTIType<CD6Joint, CJoint, CD6JointRTTI>
	{
		D6Joint::Motion& getMotion(CD6Joint* obj, UINT32 idx) { return obj->mMotion[idx]; }
		void setMotion(CD6Joint* obj, UINT32 idx, D6Joint::Motion& value) { obj->mMotion[idx] = value; }
		UINT32 getMotionCount(CD6Joint* obj) { return (UINT32)D6Joint::Motion::Count; }
		void setMotionCount(CD6Joint* obj, UINT32 size) { /* Do nothing */ }

		float& getDriveStiffness(CD6Joint* obj, UINT32 idx) { return obj->mDrive[idx].stiffness; }
		void setDriveStiffness(CD6Joint* obj, UINT32 idx, float& value) { obj->mDrive[idx].stiffness = value; }

		float& getDriveDamping(CD6Joint* obj, UINT32 idx) { return obj->mDrive[idx].damping; }
		void setDriveDamping(CD6Joint* obj, UINT32 idx, float& value) { obj->mDrive[idx].damping = value; }

		float& getDriveForceLimit(CD6Joint* obj, UINT32 idx) { return obj->mDrive[idx].forceLimit; }
		void setDriveForceLimit(CD6Joint* obj, UINT32 idx, float& value) { obj->mDrive[idx].forceLimit = value; }

		bool& getDriveAcceleration(CD6Joint* obj, UINT32 idx) { return obj->mDrive[idx].acceleration; }
		void setDriveAcceleration(CD6Joint* obj, UINT32 idx, bool& value) { obj->mDrive[idx].acceleration = value; }

		UINT32 getDriveCount(CD6Joint* obj) { return (UINT32)D6Joint::DriveType::Count; }
		void setDriveCount(CD6Joint* obj, UINT32 size) { /* Do nothing */ }

		BS_PLAIN_MEMBER_NAMED(mLimitLinearExtent, mLimitLinear.extent)
		BS_PLAIN_MEMBER_NAMED(mLimitLinearContactDist, mLimitTwist.contactDist)
		BS_PLAIN_MEMBER_NAMED(mLimitLinearRestitution, mLimitTwist.restitution)
		BS_PLAIN_MEMBER_NAMED(mLimitLinearSpringDamping, mLimitTwist.spring.damping)
		BS_PLAIN_MEMBER_NAMED(mLimitLinearSpringStiffness, mLimitTwist.spring.stiffness)

		BS_PLAIN_MEMBER_NAMED(mLimitTwistLower, mLimitTwist.lower)
		BS_PLAIN_MEMBER_NAMED(mLimitTwistUpper, mLimitTwist.upper)
		BS_PLAIN_MEMBER_NAMED(mLimitTwistContactDist, mLimitTwist.contactDist)
		BS_PLAIN_MEMBER_NAMED(mLimitTwistRestitution, mLimitTwist.restitution)
		BS_PLAIN_MEMBER_NAMED(mLimitTwistSpringDamping, mLimitTwist.spring.damping)
		BS_PLAIN_MEMBER_NAMED(mLimitTwistSpringStiffness, mLimitTwist.spring.stiffness)

		BS_PLAIN_MEMBER_NAMED(mLimitSwingYLimitAngle, mLimitSwing.yLimitAngle)
		BS_PLAIN_MEMBER_NAMED(mLimitSwingZLimitAngle, mLimitSwing.zLimitAngle)
		BS_PLAIN_MEMBER_NAMED(mLimitSwingContactDist, mLimitSwing.contactDist)
		BS_PLAIN_MEMBER_NAMED(mLimitSwingRestitution, mLimitSwing.restitution)
		BS_PLAIN_MEMBER_NAMED(mLimitSwingSpringDamping, mLimitSwing.spring.damping)
		BS_PLAIN_MEMBER_NAMED(mLimitSwingSpringStiffness, mLimitSwing.spring.stiffness)

		BS_PLAIN_MEMBER(mDrivePosition)
		BS_PLAIN_MEMBER(mDriveRotation)
		BS_PLAIN_MEMBER(mDriveLinearVelocity)
		BS_PLAIN_MEMBER(mDriveAngularVelocity)

	public:
		CD6JointRTTI()
		{
			addPlainArrayField("mMotion", 0, &CD6JointRTTI::getMotion, &CD6JointRTTI::getMotionCount, 
				&CD6JointRTTI::setMotion, &CD6JointRTTI::setMotionCount);
			addPlainArrayField("mDriveStiffnes", 1, &CD6JointRTTI::getDriveStiffness, &CD6JointRTTI::getDriveCount, 
				&CD6JointRTTI::setDriveStiffness, &CD6JointRTTI::setDriveCount);
			addPlainArrayField("mDriveDamping", 2, &CD6JointRTTI::getDriveDamping, &CD6JointRTTI::getDriveCount, 
				&CD6JointRTTI::setDriveDamping, &CD6JointRTTI::setDriveCount);
			addPlainArrayField("mDriveForceLimit", 3, &CD6JointRTTI::getDriveForceLimit, &CD6JointRTTI::getDriveCount, 
				&CD6JointRTTI::setDriveForceLimit, &CD6JointRTTI::setDriveCount);
			addPlainArrayField("mDriveAcceleartion", 4, &CD6JointRTTI::getDriveAcceleration, &CD6JointRTTI::getDriveCount, 
				&CD6JointRTTI::setDriveAcceleration, &CD6JointRTTI::setDriveCount);

			BS_ADD_PLAIN_FIELD(mLimitLinearExtent, 5);
			BS_ADD_PLAIN_FIELD(mLimitLinearContactDist, 6);
			BS_ADD_PLAIN_FIELD(mLimitLinearRestitution, 7);
			BS_ADD_PLAIN_FIELD(mLimitLinearSpringDamping, 8);
			BS_ADD_PLAIN_FIELD(mLimitLinearSpringStiffness, 9);

			BS_ADD_PLAIN_FIELD(mLimitTwistLower, 10);
			BS_ADD_PLAIN_FIELD(mLimitTwistUpper, 11);
			BS_ADD_PLAIN_FIELD(mLimitTwistContactDist, 12);
			BS_ADD_PLAIN_FIELD(mLimitTwistRestitution, 13);
			BS_ADD_PLAIN_FIELD(mLimitTwistSpringDamping, 14);
			BS_ADD_PLAIN_FIELD(mLimitTwistSpringStiffness, 15);

			BS_ADD_PLAIN_FIELD(mLimitSwingYLimitAngle, 16);
			BS_ADD_PLAIN_FIELD(mLimitSwingZLimitAngle, 17);
			BS_ADD_PLAIN_FIELD(mLimitSwingContactDist, 18);
			BS_ADD_PLAIN_FIELD(mLimitSwingRestitution, 19);
			BS_ADD_PLAIN_FIELD(mLimitSwingSpringDamping, 20);
			BS_ADD_PLAIN_FIELD(mLimitSwingSpringStiffness, 21);

			BS_ADD_PLAIN_FIELD(mDrivePosition, 22);
			BS_ADD_PLAIN_FIELD(mDriveRotation, 23);
			BS_ADD_PLAIN_FIELD(mDriveLinearVelocity, 24);
			BS_ADD_PLAIN_FIELD(mDriveAngularVelocity, 25);
		}

		const String& getRTTIName() override
		{
			static String name = "CD6Joint";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CD6Joint;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CD6Joint>();
		}
	};

	/** @} */
	/** @endcond */
}