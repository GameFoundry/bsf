//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsFJoint.h"
#include "extensions\PxJoint.h"

namespace BansheeEngine
{
	class BS_PHYSX_EXPORT FPhysXJoint : public FJoint
	{
	public:
		FPhysXJoint(physx::PxJoint* joint);
		~FPhysXJoint();

		Rigidbody* getBody(JointBody body) const override;
		void setBody(JointBody body, Rigidbody* value) override;

		Vector3 getPosition(JointBody body) const override;
		Quaternion getRotation(JointBody body) const override;
		void setTransform(JointBody body, const Vector3& position, const Quaternion& rotation) override;

		float getBreakForce() const override;
		void setBreakForce(float force) override;

		float getBreakTorque() const override;
		void setBreakToque(float torque) override;

		physx::PxJoint* _getInternal() const { return mJoint; }
	protected:
		physx::PxJoint* mJoint;
	};
}