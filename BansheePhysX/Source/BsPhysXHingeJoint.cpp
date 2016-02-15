//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPhysXHingeJoint.h"
#include "BsFPhysxJoint.h"

using namespace physx;

namespace BansheeEngine
{
	PxRevoluteJointFlag::Enum toPxFlag(PhysXHingeJoint::Flag flag)
	{
		switch (flag)
		{
		case PhysXHingeJoint::Flag::Limit:
			return PxRevoluteJointFlag::eLIMIT_ENABLED;
		default:
		case PhysXHingeJoint::Flag::Drive:
			return PxRevoluteJointFlag::eDRIVE_ENABLED;
		}
	}

	PhysXHingeJoint::PhysXHingeJoint(PxPhysics* physx)
	{
		PxRevoluteJoint* joint = PxRevoluteJointCreate(*physx, nullptr, PxTransform(), nullptr, PxTransform());

		mInternal = bs_new<FPhysXJoint>(joint);
	}

	PhysXHingeJoint::~PhysXHingeJoint()
	{
		bs_delete(mInternal);
	}

	Radian PhysXHingeJoint::getAngle() const
	{
		return Radian(getInternal()->getAngle());
	}

	float PhysXHingeJoint::getSpeed() const
	{
		return getInternal()->getVelocity();
	}

	LimitAngular PhysXHingeJoint::getLimit() const
	{
		PxJointAngularLimitPair pxLimit = getInternal()->getLimit();

		LimitAngular limit;
		limit.lower = pxLimit.lower;
		limit.upper = pxLimit.upper;
		limit.contactDist = pxLimit.contactDistance;
		limit.spring.stiffness = pxLimit.stiffness;
		limit.spring.damping = pxLimit.damping;

		return limit;
	}

	void PhysXHingeJoint::setLimit(const LimitAngular& limit) const
	{
		PxJointAngularLimitPair pxLimit(limit.lower.valueRadians(), limit.upper.valueRadians(), limit.contactDist);
		pxLimit.stiffness = limit.spring.stiffness;
		pxLimit.damping = limit.spring.damping;

		getInternal()->setLimit(pxLimit);
	}

	PhysXHingeJoint::Drive PhysXHingeJoint::getDrive() const
	{
		Drive drive;
		drive.speed = getInternal()->getDriveVelocity();
		drive.forceLimit = getInternal()->getDriveForceLimit();
		drive.gearRatio = getInternal()->getDriveGearRatio();
		drive.freeSpin = getInternal()->getRevoluteJointFlags() & PxRevoluteJointFlag::eDRIVE_FREESPIN;

		return drive;
	}

	void PhysXHingeJoint::setDrive(const Drive& drive) const
	{
		getInternal()->setDriveVelocity(drive.speed);
		getInternal()->setDriveForceLimit(drive.forceLimit);
		getInternal()->setDriveGearRatio(drive.gearRatio);
		getInternal()->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_FREESPIN, drive.freeSpin);
	}

	void PhysXHingeJoint::setFlag(Flag flag, bool enabled)
	{
		getInternal()->setRevoluteJointFlag(toPxFlag(flag), enabled);
	}

	bool PhysXHingeJoint::hasFlag(Flag flag)
	{
		return getInternal()->getRevoluteJointFlags() & toPxFlag(flag);
	}

	PxRevoluteJoint* PhysXHingeJoint::getInternal() const
	{
		FPhysXJoint* internal = static_cast<FPhysXJoint*>(mInternal);

		return static_cast<PxRevoluteJoint*>(internal->_getInternal());
	}
}