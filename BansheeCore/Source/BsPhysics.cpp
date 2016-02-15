//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPhysics.h"
#include "BsRigidbody.h"

namespace BansheeEngine
{
	Physics::Physics()
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

		UINT32 lastId = (UINT32)rigidbodies.size();
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