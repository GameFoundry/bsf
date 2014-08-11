#include "BsManagedResourceManager.h"
#include "BsResources.h"

namespace BansheeEngine
{
	ManagedResourceManager::ManagedResourceManager()
	{

	}

	ManagedResourceManager::~ManagedResourceManager()
	{
		UnorderedMap<String, HManagedResource> resourceCopy = mResources;
		for (auto& resourcePair : resourceCopy)
		{
			HManagedResource resource = resourcePair.second;
			if (resource != nullptr && resource.isLoaded())
				gResources().unload(resource);
		}

		mResources.clear();
	}

	void ManagedResourceManager::registerManagedResource(const HManagedResource& resource)
	{
		mResources.insert(std::make_pair(resource.getUUID(), resource));
	}

	void ManagedResourceManager::unregisterManagedResource(const HManagedResource& resource)
	{
		mResources.erase(resource.getUUID());
	}
}