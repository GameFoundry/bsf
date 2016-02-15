//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPhysXDistanceJoint.h"
#include "BsFPhysxJoint.h"

using namespace physx;

namespace BansheeEngine
{
	PxDistanceJointFlag::Enum toPxFlag(PhysXDistanceJoint::Flag flag)
	{
		switch (flag)
		{
		case PhysXDistanceJoint::Flag::MaxDistance:
			return PxDistanceJointFlag::eMAX_DISTANCE_ENABLED;
		case PhysXDistanceJoint::Flag::MinDistance:
			return PxDistanceJointFlag::eMIN_DISTANCE_ENABLED;
		default:
		case PhysXDistanceJoint::Flag::Spring:
			return PxDistanceJointFlag::eSPRING_ENABLED;
		}
	}

	PhysXDistanceJoint::PhysXDistanceJoint(PxPhysics* physx)
	{
		PxDistanceJoint* joint = PxDistanceJointCreate(*physx, nullptr, PxTransform(), nullptr, PxTransform());

		mInternal = bs_new<FPhysXJoint>(joint);
	}

	PhysXDistanceJoint::~PhysXDistanceJoint()
	{
		bs_delete(mInternal);
	}

	float PhysXDistanceJoint::getDistance() const
	{
		return getInternal()->getDistance();
	}

	float PhysXDistanceJoint::getMinDistance() const
	{
		return getInternal()->getMinDistance();
	}

	void PhysXDistanceJoint::setMinDistance(float value)
	{
		getInternal()->setMinDistance(value);
	}

	float PhysXDistanceJoint::getMaxDistance() const
	{
		return getInternal()->getMaxDistance();
	}

	void PhysXDistanceJoint::setMaxDistance(float value)
	{
		getInternal()->setMaxDistance(value);
	}

	float PhysXDistanceJoint::getTolerance() const
	{
		return getInternal()->getTolerance();
	}

	void PhysXDistanceJoint::setTolerance(float value)
	{
		getInternal()->setTolerance(value);
	}

	Spring PhysXDistanceJoint::getSpring() const
	{
		float damping = getInternal()->getDamping();
		float stiffness = getInternal()->getStiffness();

		return Spring(stiffness, damping);
	}

	void PhysXDistanceJoint::setSpring(const Spring& value)
	{
		getInternal()->setDamping(value.damping);
		getInternal()->setStiffness(value.stiffness);
	}

	void PhysXDistanceJoint::setFlag(Flag flag, bool enabled)
	{
		getInternal()->setDistanceJointFlag(toPxFlag(flag), enabled);
	}

	bool PhysXDistanceJoint::hasFlag(Flag flag)
	{
		return getInternal()->getDistanceJointFlags() & toPxFlag(flag);
	}

	PxDistanceJoint* PhysXDistanceJoint::getInternal() const
	{
		FPhysXJoint* internal = static_cast<FPhysXJoint*>(mInternal);

		return static_cast<PxDistanceJoint*>(internal->_getInternal());
	}
}