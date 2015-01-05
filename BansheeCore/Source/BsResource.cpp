#include "BsResource.h"
#include "BsResourceRTTI.h"
#include "BsResources.h"
#include "BsUUID.h"
#include "BsResourceMetaData.h"
#include "BsRenderAPI.h"

namespace BansheeEngine
{
	Resource::Resource(bool initializeOnRenderThread)
		:CoreObject(initializeOnRenderThread), mSize(0)
	{ 
		mMetaData = bs_shared_ptr<ResourceMetaData>();
	}

	const WString& Resource::getName() const 
	{ 
		return mMetaData->displayName; 
	}

	void Resource::setName(const WString& name) 
	{ 
		mMetaData->displayName = name; 
	}

	bool Resource::areDependenciesLoaded() const
	{
		Vector<HResource> dependencies;
		getResourceDependencies(dependencies);

		for (auto& dependency : dependencies)
		{
			if (dependency != nullptr && !dependency.isLoaded())
				return false;
		}

		return true;
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