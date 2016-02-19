//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsCollision.h"
#include "BsFCollider.h"
#include "PxRigidStatic.h"

namespace BansheeEngine
{
	/** @addtogroup PhysX
	 *  @{
	 */

	/** PhysX implementation of FCollider. */
	class FPhysXCollider : public FCollider
	{
	public:
		explicit FPhysXCollider(physx::PxShape* shape);
		~FPhysXCollider();

		/** @copydoc FCollider::getPosition */
		Vector3 getPosition() const override;

		/** @copydoc FCollider::getRotation */
		Quaternion getRotation() const override;

		/** @copydoc FCollider::setTransform */
		void setTransform(const Vector3& pos, const Quaternion& rotation) override;

		/** @copydoc FCollider::setIsTrigger */
		void setIsTrigger(bool value) override;

		/** @copydoc FCollider::getIsTrigger */
		bool getIsTrigger() const override;

		/** @copydoc FCollider::setIsStatic */
		void setIsStatic(bool value) override;

		/** @copydoc FCollider::getIsStatic */
		bool getIsStatic() const override;

		/** @copydoc FCollider::setContactOffset */
		void setContactOffset(float value) override;

		/** @copydoc FCollider::getContactOffset */
		float getContactOffset() const override;

		/** @copydoc FCollider::setRestOffset */
		void setRestOffset(float value) override;

		/** @copydoc FCollider::getRestOffset */
		float getRestOffset() const override;

		/** @copydoc FCollider::setMaterial */
		void setMaterial(const HPhysicsMaterial& material) override;

		/** @copydoc FCollider::setLayer */
		void setLayer(UINT64 layer) override;

		/** Gets the internal PhysX shape that represents the collider. */
		physx::PxShape* _getShape() const { return mShape; }
	protected:
		physx::PxShape* mShape;
		physx::PxRigidStatic* mStaticBody;
		bool mIsTrigger;
		bool mIsStatic;
	};

	/** @} */
}