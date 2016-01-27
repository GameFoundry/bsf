#include "BsPhysXPrerequisites.h"
#include "BsPhysicsManager.h"
#include "BsPhysX.h"

namespace BansheeEngine
{
	class BS_PHYSX_EXPORT PhysXFactory : public PhysicsFactory
	{
	public:
		void startUp() override
		{
			Physics::startUp<PhysX>();
		}

		void shutDown() override
		{
			Physics::shutDown();
		}
	};

	extern "C" BS_PHYSX_EXPORT PhysXFactory* loadPlugin()
	{
		return bs_new<PhysXFactory>();
	}

	extern "C" BS_PHYSX_EXPORT void unloadPlugin(PhysXFactory* instance)
	{
		bs_delete(instance);
	}
}