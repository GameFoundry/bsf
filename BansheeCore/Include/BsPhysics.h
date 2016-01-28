#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT Physics : public Module<Physics>
	{
	public:
		Physics();
		virtual ~Physics() { }

		virtual void update() = 0;

		void toggleCollision(UINT64 groupA, UINT64 groupB, bool enabled);
		bool isCollisionEnabled(UINT64 groupA, UINT64 groupB) const;

		static const UINT64 CollisionMapSize = 64;
	protected:
		mutable Mutex mMutex;
		bool mCollisionMap[CollisionMapSize][CollisionMapSize];
	};
}