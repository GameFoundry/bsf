//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsMeshCollider.h"
#include "PxPhysics.h"

namespace BansheeEngine
{
	class PhysXMeshCollider : public MeshCollider
	{
	public:
		PhysXMeshCollider(physx::PxPhysics* physx, const Vector3& position, const Quaternion& rotation);
		~PhysXMeshCollider();

		void setScale(const Vector3& scale) override;

	private:
		FPhysXCollider* getInternal() const;

		void onMeshChanged() override;
		void applyGeometry();
	};
}