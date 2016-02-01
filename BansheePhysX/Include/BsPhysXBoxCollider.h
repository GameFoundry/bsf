//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsBoxCollider.h"
#include "PxPhysics.h"

namespace BansheeEngine
{
	class PhysXBoxCollider : public BoxCollider
	{
	public:
		PhysXBoxCollider(physx::PxPhysics* physx, const Vector3& position, const Quaternion& rotation, 
			float extentX, float extentY, float extentZ);
		~PhysXBoxCollider();

	private:
		FPhysXCollider* getInternal() const;
	};
}