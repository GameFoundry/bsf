//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsSphereCollider.h"
#include "PxPhysics.h"

namespace BansheeEngine
{
	class PhysXSphereCollider : public SphereCollider
	{
	public:
		PhysXSphereCollider(physx::PxPhysics* physx, const Vector3& position, const Quaternion& rotation, float radius);
		~PhysXSphereCollider();

		void setRadius(float radius) override;
		float getRadius() const override;

	private:
		FPhysXCollider* getInternal() const;
	};
}