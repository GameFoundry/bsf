//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT PhysicsFactory
	{
	public:
		virtual ~PhysicsFactory() { }

		virtual void startUp(bool cooking) = 0;
		virtual void shutDown() = 0;
	};

	class BS_CORE_EXPORT PhysicsManager : public Module<PhysicsManager>
	{
	public:
		PhysicsManager(const String& pluginName, bool cooking);
		~PhysicsManager();

	private:
		DynLib* mPlugin;
		PhysicsFactory* mFactory;
	};
}