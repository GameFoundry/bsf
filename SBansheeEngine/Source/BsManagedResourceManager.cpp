#include "BsManagedResourceManager.h"
#include "BsResources.h"

namespace BansheeEngine
{
	ManagedResourceManager::ManagedResourceManager()
	{

	}

	ManagedResourceManager::~ManagedResourceManager()
	{
		clear();
	}

	void ManagedResourceManager::clear()
	{
		UnorderedMap<String, WeakResourceHandle<ManagedResource>> resourceCopy = mResources;
		for (auto& resourcePair : resourceCopy)
		{
			WeakResourceHandle<ManagedResource> resource = resourcePair.second;
			gResources().release((WeakResourceHandle<Resource>&)resource);
		}

		mResources.clear();
	}

	void ManagedResourceManager::registerManagedResource(const WeakResourceHandle<ManagedResource>& resource)
	{
		mResources.insert(std::make_pair(resource.getUUID(), resource));
	}

	void ManagedResourceManager::unregisterManagedResource(const WeakResourceHandle<ManagedResource>& resource)
	{
		mResources.erase(resource.getUUID());
	}
}