//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Resources/BsResource.h"
#include "Private/RTTI/BsResourceRTTI.h"
#include "Resources/BsResourceMetaData.h"

namespace bs
{
	Resource::Resource(bool initializeOnRenderThread)
		:CoreObject(initializeOnRenderThread), mSize(0), mKeepSourceData(true)
	{
		mMetaData = bs_shared_ptr_new<ResourceMetaData>();
	}

	const String& Resource::getName() const
	{
		return mMetaData->displayName;
	}

	void Resource::setName(const String& name)
	{
		mMetaData->displayName = name;
	}

	void Resource::getResourceDependencies(FrameVector<HResource>& dependencies) const
	{
		Lock lock(mDependenciesMutex);

		for(auto& dependency : mDependencies)
		{
			if(dependency != nullptr)
				dependencies.push_back(static_resource_cast<Resource>(dependency));
		}
	}

	bool Resource::areDependenciesLoaded() const
	{
		Lock lock(mDependenciesMutex);
		bs_frame_mark();

		bool areLoaded = true;
		{
			for (auto& dependency : mDependencies)
			{
				if (dependency != nullptr && !dependency.isLoaded())
				{
					areLoaded = false;
					break;
				}
			}
		}

		bs_frame_clear();
		return areLoaded;
	}

	void Resource::addResourceDependency(const HResource& resource)
	{
		if(resource == nullptr)
			return;

		Lock lock(mDependenciesMutex);
		mDependencies.push_back(resource.getWeak());
	}

	void Resource::removeResourceDependency(const HResource& resource)
	{
		Lock lock(mDependenciesMutex);
		mDependencies.erase(std::remove(mDependencies.begin(), mDependencies.end(), resource.getWeak()),
			mDependencies.end());
	}

	RTTITypeBase* Resource::getRTTIStatic()
	{
		return ResourceRTTI::instance();
	}

	RTTITypeBase* Resource::getRTTI() const
	{
		return Resource::getRTTIStatic();
	}
}
