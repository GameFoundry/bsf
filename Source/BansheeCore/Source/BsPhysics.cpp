//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPhysics.h"
#include "BsRigidbody.h"
#include "BsRay.h"
#include "BsCCollider.h"

namespace BansheeEngine
{
	Physics::Physics(const PHYSICS_INIT_DESC& init)
	{
		memset(mCollisionMap, 1, CollisionMapSize * CollisionMapSize * sizeof(bool));
	}

	void Physics::toggleCollision(UINT64 groupA, UINT64 groupB, bool enabled)
	{
		assert(groupA < CollisionMapSize && groupB < CollisionMapSize);

		mMutex.lock();
		mCollisionMap[groupA][groupB] = enabled;
	}

	bool Physics::isCollisionEnabled(UINT64 groupA, UINT64 groupB) const
	{
		assert(groupA < CollisionMapSize && groupB < CollisionMapSize);

		mMutex.lock();
		return mCollisionMap[groupA][groupB];
	}

	bool Physics::rayCast(const Ray& ray, PhysicsQueryHit& hit, UINT64 layer, float max) const
	{
		return rayCast(ray.getOrigin(), ray.getDirection(), hit, layer, max);
	}

	Vector<PhysicsQueryHit> Physics::rayCastAll(const Ray& ray, UINT64 layer, float max) const
	{
		return rayCastAll(ray.getOrigin(), ray.getDirection(), layer, max);
	}

	bool Physics::rayCastAny(const Ray& ray, UINT64 layer, float max) const
	{
		return rayCastAny(ray.getOrigin(), ray.getDirection(), layer, max);
	}

	Vector<HCollider> rawToComponent(const Vector<Collider*>& raw)
	{
		if (raw.empty())
			return Vector<HCollider>(0);

		Vector<HCollider> output;
		for (auto& entry : raw)
		{
			if (entry == nullptr)
				continue;

			CCollider* component = (CCollider*)entry->_getOwner(PhysicsOwnerType::Component);
			if (component == nullptr)
				continue;

			output.push_back(component->getHandle());
		}

		return output;
	}

	Vector<HCollider> Physics::boxOverlap(const AABox& box, const Quaternion& rotation, UINT64 layer) const
	{
		return rawToComponent(_boxOverlap(box, rotation, layer));
	}

	Vector<HCollider> Physics::sphereOverlap(const Sphere& sphere, UINT64 layer) const
	{
		return rawToComponent(_sphereOverlap(sphere, layer));
	}

	Vector<HCollider> Physics::capsuleOverlap(const Capsule& capsule, const Quaternion& rotation, UINT64 layer) const
	{
		return rawToComponent(_capsuleOverlap(capsule, rotation, layer));
	}

	Vector<HCollider> Physics::convexOverlap(const HPhysicsMesh& mesh, const Vector3& position,
		const Quaternion& rotation, UINT64 layer) const
	{
		return rawToComponent(_convexOverlap(mesh, position, rotation, layer));
	}

	void Physics::registerRigidbody(Rigidbody* body, UINT32 priority)
	{
		assert(priority <= MAX_PRIORITY && "Priority value too high");

		if (mRigidbodies.size() <= priority)
			mRigidbodies.resize(priority + 1);

		UINT32 nextId = (UINT32)mRigidbodies[priority].size();
		mRigidbodies[priority].push_back(body);
		body->_setPhysicsId(nextId);
	}

	void Physics::unregisterRigidbody(UINT32 id, UINT32 priority)
	{
		assert(mRigidbodies.size() >= priority);

		auto& rigidbodies = mRigidbodies[priority];
		assert(rigidbodies.size() > 0);

		UINT32 lastId = (UINT32)(rigidbodies.size() - 1);
		if (id != lastId)
		{
			rigidbodies[id] = rigidbodies[lastId];
			rigidbodies[id]->_setPhysicsId(id);
		}

		rigidbodies.erase(rigidbodies.begin() + lastId);
	}

	void Physics::updatePriority(UINT32 id, UINT32 oldPriority, UINT32 newPriority)
	{
		assert(newPriority <= MAX_PRIORITY && "Priority value too high");

		if (oldPriority == newPriority)
			return;

		assert(mRigidbodies.size() >= oldPriority);

		auto& rigidbodies = mRigidbodies[oldPriority];
		Rigidbody* rigidbody = rigidbodies[id];

		unregisterRigidbody(id, oldPriority);
		registerRigidbody(rigidbody, newPriority);
	}

	Physics& gPhysics()
	{
		return Physics::instance();
	}
}