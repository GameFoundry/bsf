//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPhysXSphericalJoint.h"
#include "BsFPhysxJoint.h"

using namespace physx;

namespace BansheeEngine
{
	PxSphericalJointFlag::Enum toPxFlag(PhysXSphericalJoint::Flag flag)
	{
		switch (flag)
		{
		default:
		case PhysXSphericalJoint::Flag::Limit:
			return PxSphericalJointFlag::eLIMIT_ENABLED;
		}
	}

	PhysXSphericalJoint::PhysXSphericalJoint(PxPhysics* physx)
	{
		PxSphericalJoint* joint = PxSphericalJointCreate(*physx, nullptr, PxTransform(), nullptr, PxTransform());
		joint->userData = this;

		mInternal = bs_new<FPhysXJoint>(joint);
	}

	PhysXSphericalJoint::~PhysXSphericalJoint()
	{
		bs_delete(mInternal);
	}

	LimitConeRange PhysXSphericalJoint::getLimit() const
	{
		PxJointLimitCone pxLimit = getInternal()->getLimitCone();

		LimitConeRange limit;
		limit.yLimitAngle = pxLimit.yAngle;
		limit.zLimitAngle = pxLimit.zAngle;
		limit.contactDist = pxLimit.contactDistance;
		limit.restitution = pxLimit.restitution;
		limit.spring.stiffness = pxLimit.stiffness;
		limit.spring.damping = pxLimit.damping;

		return limit;
	}

	void PhysXSphericalJoint::setLimit(const LimitConeRange& limit)
	{
		PxJointLimitCone pxLimit(limit.yLimitAngle.valueRadians(), limit.zLimitAngle.valueRadians(), limit.contactDist);
		pxLimit.stiffness = limit.spring.stiffness;
		pxLimit.damping = limit.spring.damping;
		pxLimit.restitution = limit.restitution;

		getInternal()->setLimitCone(pxLimit);
	}

	void PhysXSphericalJoint::setFlag(Flag flag, bool enabled)
	{
		getInternal()->setSphericalJointFlag(toPxFlag(flag), enabled);
	}

	bool PhysXSphericalJoint::hasFlag(Flag flag) const
	{
		return getInternal()->getSphericalJointFlags() & toPxFlag(flag);
	}

	PxSphericalJoint* PhysXSphericalJoint::getInternal() const
	{
		FPhysXJoint* internal = static_cast<FPhysXJoint*>(mInternal);

		return static_cast<PxSphericalJoint*>(internal->_getInternal());
	}
}