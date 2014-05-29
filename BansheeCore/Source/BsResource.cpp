#include "BsResource.h"
#include "BsResourceRTTI.h"
#include "BsResources.h"
#include "BsUUID.h"
#include "BsRenderSystem.h"

namespace BansheeEngine
{
	Resource::Resource(bool initializeOnRenderThread)
		:CoreObject(initializeOnRenderThread), mSize(0)
	{ }

	RTTITypeBase* Resource::getRTTIStatic()
	{
		return ResourceRTTI::instance();
	}

	RTTITypeBase* Resource::getRTTI() const
	{
		return Resource::getRTTIStatic();
	}
}