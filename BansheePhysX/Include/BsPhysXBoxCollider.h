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
			const Vector3& extents);
		~PhysXBoxCollider();

		void setScale(const Vector3& scale) override;

		void setExtents(const Vector3& extents) override;
		Vector3 getExtents() const override;

	private:
		FPhysXCollider* getInternal() const;
		void applyGeometry();

		Vector3 mExtents;
	};
}