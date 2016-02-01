//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsCollision.h"
#include "BsFCollider.h"
#include "PxRigidStatic.h"

namespace BansheeEngine
{
	class FPhysXCollider : public FCollider
	{
	public:
		explicit FPhysXCollider(physx::PxShape* shape);
		~FPhysXCollider();

		Vector3 getPosition() const override;
		Quaternion getRotation() const override;
		void setTransform(const Vector3& pos, const Quaternion& rotation) override;

		void setIsTrigger(bool value) override;
		bool getIsTrigger() const override;

		void setContactOffset(float value) override;
		float getContactOffset() override;

		void setRestOffset(float value) override;
		float getRestOffset() override;

		void setRigidbody(const SPtr<Rigidbody>& rigidbody) override;
		void setMaterial(const HPhysicsMaterial& material) override;
		void setIsActive(bool value) override;
		void setLayer(UINT64 layer) override;

	protected:
		physx::PxShape* mShape;
		physx::PxRigidStatic* mStaticBody;
		bool mIsTrigger;
	};
}