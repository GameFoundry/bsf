//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsPhysicsCommon.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT Collider
	{
	public:
		virtual ~Collider() { }

		inline Vector3 getPosition() const;
		inline Quaternion getRotation() const;
		inline void setTransform(const Vector3& pos, const Quaternion& rot);

		virtual void setScale(const Vector3& scale);
		inline Vector3 getScale() const;

		inline void setIsTrigger(bool value);
		inline bool getIsTrigger() const;

		inline void setRigidbody(Rigidbody* value);
		Rigidbody* getRigidbody() const { return mRigidbody; }

		inline void setMass(float mass);
		inline float getMass() const;

		inline void setMaterial(const HPhysicsMaterial& material);
		inline HPhysicsMaterial getMaterial() const;

		inline void setContactOffset(float value);
		inline float getContactOffset();

		inline void setRestOffset(float value);
		inline float getRestOffset();

		inline void setLayer(UINT64 layer);
		inline UINT64 getLayer() const;

		/** 
		 * Checks does the ray hit this collider. 
		 *
		 * @param[in]	ray		Ray to check.
		 * @param[out]	hit		Information about the hit. Valid only if the method returns true.
		 * @param[in]	maxDist	Maximum distance from the ray origin to search for hits.
		 * @return				True if the ray has hit the collider.
		 */
		inline bool rayCast(const Ray& ray, PhysicsQueryHit& hit, float maxDist = FLT_MAX) const;

		/** 
		 * Checks does the ray hit this collider. 
		 *
		 * @param[in]	origin	Origin of the ray to check.
		 * @param[in]	unitDir	Unit direction of the ray to check.
		 * @param[out]	hit		Information about the hit. Valid only if the method returns true.
		 * @param[in]	maxDist	Maximum distance from the ray origin to search for hits.
		 * @return				True if the ray has hit the collider.
		 */
		inline bool rayCast(const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit,
			float maxDist = FLT_MAX) const;

		Event<void(const CollisionData&)> onCollisionBegin;
		Event<void(const CollisionData&)> onCollisionStay;
		Event<void(const CollisionData&)> onCollisionEnd;

		/** @cond INTERNAL */

		FCollider* _getInternal() const { return mInternal; }

		/** 
		 * Sets the object that owns this physics object, if any. Used for high level systems so they can easily map their
		 * high level physics objects from the low level ones returned by various queries and events.
		 */
		void _setOwner(PhysicsOwnerType type, void* owner) { mOwner.type = type; mOwner.ownerData = owner; }

		/** 
		 * Gets the object that owns this physics object, if any. Used for high level systems so they can easily map their
		 * high level physics objects from the low level ones returned by various queries and events.
		 */
		void* _getOwner(PhysicsOwnerType type) const { return mOwner.type == type ? mOwner.ownerData : nullptr; }

		/** @endcond */
	protected:
		FCollider* mInternal = nullptr;
		PhysicsObjectOwner mOwner;
		Rigidbody* mRigidbody = nullptr;
		Vector3 mScale = Vector3::ONE;
	};
}