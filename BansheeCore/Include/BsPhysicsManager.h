#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT PhysicsFactory
	{
	public:
		virtual ~PhysicsFactory() { }

		virtual void startUp() = 0;
		virtual void shutDown() = 0;
	};

	class BS_CORE_EXPORT PhysicsManager : public Module<PhysicsManager>
	{
	public:
		PhysicsManager(const String& pluginName);
		~PhysicsManager();

	private:
		DynLib* mPlugin;
		PhysicsFactory* mFactory;
	};
}