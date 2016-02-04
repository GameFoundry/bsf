//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT Physics : public Module<Physics>
	{
	public:
		Physics();
		virtual ~Physics() { }

		virtual void update() = 0;

		virtual SPtr<PhysicsMaterial> createMaterial(float staticFriction, float dynamicFriction, float restitution) = 0;
		virtual SPtr<Rigidbody> createRigidbody(const Vector3& position, const Quaternion& rotation, UINT32 priority = 0) = 0;

		virtual SPtr<BoxCollider> createBoxCollider(float extentX, float extentY, float extentZ,
			const Vector3& position, const Quaternion& rotation) = 0;
		virtual SPtr<SphereCollider> createSphereCollider(float radius,
			const Vector3& position, const Quaternion& rotation) = 0;
		virtual SPtr<PlaneCollider> createPlaneCollider(const Vector3& position, const Quaternion& rotation) = 0;
		virtual SPtr<CapsuleCollider> createCapsuleCollider(float radius, float halfHeight,
			const Vector3& position, const Quaternion& rotation) = 0;

		void toggleCollision(UINT64 groupA, UINT64 groupB, bool enabled);
		bool isCollisionEnabled(UINT64 groupA, UINT64 groupB) const;

		static const UINT64 CollisionMapSize = 64;
	protected:
		friend class Rigidbody;

		void registerRigidbody(Rigidbody* body, UINT32 priority);
		void unregisterRigidbody(UINT32 id, UINT32 priority);
		void updatePriority(UINT32 id, UINT32 oldPriority, UINT32 newPriority);

		mutable Mutex mMutex;
		bool mCollisionMap[CollisionMapSize][CollisionMapSize];

		Vector<Vector<Rigidbody*>> mRigidbodies;

		const static UINT32 MAX_PRIORITY = 128;
	};

	/** Provides easier access to Physics. */
	BS_CORE_EXPORT Physics& gPhysics();
}