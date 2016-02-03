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

		void setMass(float mass) override;

		/**
		 * Determines how far apart do two shapes need to be away from each other before the physics runtime starts 
		 * generating repelling impulse for them. This distance will be the sum of contact offsets of the two interacting
		 * objects. If objects are moving fast you can increase this value to start generating the impulse earlier and 
		 * potentially prevent the objects from interpenetrating. This value is in meters.
		 *
		 * Also see setRestOffset().
		 */
		void setContactOffset(float value) override;

		/** Returns shape's contact offset in meters. See setContactOffset() to learn contact offset is. */
		float getContactOffset() override;

		/**
		 * Sets at what distance should two objects resting on one another come to an equilibrium. The value used in the
		 * runtime will be the sum of rest offsets for both interacting objects. This value is in meters.
		 */
		void setRestOffset(float value) override;

		/** Returns shepe's rest offset in meters. See setRestOffset() to learn what contact offset is. */
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