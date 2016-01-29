#include "BsPhysics.h"

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

	Physics& gPhysics()
	{
		return Physics::instance();
	}
}