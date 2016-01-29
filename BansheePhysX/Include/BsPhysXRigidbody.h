//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsRigidbody.h"
#include "PxPhysics.h"

namespace BansheeEngine
{
	class PhysXRigidbody : public Rigidbody
	{
	public:
		PhysXRigidbody(physx::PxPhysics* physx, const Vector3& position, const Quaternion& rotation);
		~PhysXRigidbody();

		// TODO

	private:
		physx::PxRigidDynamic* mInternal;
	};
}