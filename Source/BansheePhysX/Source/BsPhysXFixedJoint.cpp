//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPhysXFixedJoint.h"
#include "BsFPhysxJoint.h"
#include "extensions\PxFixedJoint.h"

using namespace physx;

namespace BansheeEngine
{
	PhysXFixedJoint::PhysXFixedJoint(PxPhysics* physx)
	{
		PxFixedJoint* joint = PxFixedJointCreate(*physx, nullptr, PxTransform(PxIdentity), nullptr, PxTransform(PxIdentity));
		joint->userData = this;

		mInternal = bs_new<FPhysXJoint>(joint);
	}

	PhysXFixedJoint::~PhysXFixedJoint()
	{
		bs_delete(mInternal);
	}
}