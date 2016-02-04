//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsCapsuleCollider.h"
#include "PxPhysics.h"

namespace BansheeEngine
{
	class PhysXCapsuleCollider : public CapsuleCollider
	{
	public:
		PhysXCapsuleCollider(physx::PxPhysics* physx, const Vector3& position, const Quaternion& rotation, 
			float radius, float halfHeight);
		~PhysXCapsuleCollider();

		/** @copydoc CapsuleCollider::setHalfHeight() */
		void setHalfHeight(float halfHeight) override;

		/** @copydoc CapsuleCollider::getHalfHeight() */
		float getHalfHeight() const override;

		/** @copydoc CapsuleCollider::setRadius() */
		void setRadius(float radius) override;

		/** @copydoc CapsuleCollider::getRadius() */
		float getRadius() const override;

	private:
		FPhysXCollider* getInternal() const;
	};
}