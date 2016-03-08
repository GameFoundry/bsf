//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPhysXSliderJoint.h"
#include "BsFPhysxJoint.h"
#include "BsPhysX.h"

using namespace physx;

namespace BansheeEngine
{
	PxPrismaticJointFlag::Enum toPxFlag(PhysXSliderJoint::Flag flag)
	{
		switch (flag)
		{
		default:
		case PhysXSliderJoint::Flag::Limit:
			return PxPrismaticJointFlag::eLIMIT_ENABLED;
		}
	}

	PhysXSliderJoint::PhysXSliderJoint(PxPhysics* physx)
	{
		PxPrismaticJoint* joint = PxPrismaticJointCreate(*physx, nullptr, PxTransform(PxIdentity), nullptr, PxTransform(PxIdentity));
		joint->userData = this;

		mInternal = bs_new<FPhysXJoint>(joint);
	}

	PhysXSliderJoint::~PhysXSliderJoint()
	{
		bs_delete(mInternal);
	}

	float PhysXSliderJoint::getPosition() const
	{
		return getInternal()->getPosition();
	}

	float PhysXSliderJoint::getSpeed() const
	{
		return getInternal()->getVelocity();
	}

	LimitLinearRange PhysXSliderJoint::getLimit() const
	{
		PxJointLinearLimitPair pxLimit = getInternal()->getLimit();

		LimitLinearRange limit;
		limit.lower = pxLimit.lower;
		limit.upper = pxLimit.upper;
		limit.contactDist = pxLimit.contactDistance;
		limit.restitution = pxLimit.restitution;
		limit.spring.stiffness = pxLimit.stiffness;
		limit.spring.damping = pxLimit.damping;

		return limit;
	}

	void PhysXSliderJoint::setLimit(const LimitLinearRange& limit)
	{
		PxJointLinearLimitPair pxLimit(gPhysX().getScale(), limit.lower, limit.upper, limit.contactDist);
		pxLimit.stiffness = limit.spring.stiffness;
		pxLimit.damping = limit.spring.damping;
		pxLimit.restitution = limit.restitution;

		getInternal()->setLimit(pxLimit);
	}

	void PhysXSliderJoint::setFlag(Flag flag, bool enabled)
	{
		getInternal()->setPrismaticJointFlag(toPxFlag(flag), enabled);
	}

	bool PhysXSliderJoint::hasFlag(Flag flag) const
	{
		return getInternal()->getPrismaticJointFlags() & toPxFlag(flag);
	}

	PxPrismaticJoint* PhysXSliderJoint::getInternal() const
	{
		FPhysXJoint* internal = static_cast<FPhysXJoint*>(mInternal);

		return static_cast<PxPrismaticJoint*>(internal->_getInternal());
	}
}