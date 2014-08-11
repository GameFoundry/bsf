#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedResourceManager : public Module <ManagedResourceManager>
	{
	public:
		ManagedResourceManager();
		~ManagedResourceManager();

		void registerManagedResource(const HManagedResource& resource);
		void unregisterManagedResource(const HManagedResource& resource);

	private:
		UnorderedMap<String, HManagedResource> mResources;
	};
}