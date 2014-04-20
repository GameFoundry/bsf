#include "CmResource.h"
#include "CmResourceRTTI.h"
#include "CmResources.h"
#include "CmUUID.h"
#include "CmRenderSystem.h"

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