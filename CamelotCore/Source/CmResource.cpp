#include "CmResource.h"
#include "CmResourceRTTI.h"
#include "CmUUID.h"
#include "CmRenderSystem.h"

namespace CamelotFramework
{
	Resource::Resource(bool initializeOnRenderThread)
		:CoreObject(initializeOnRenderThread), mSize(0)
	{ }

	HResource Resource::_createResourceHandle(ResourcePtr obj)
	{
		return HResource(obj, UUIDGenerator::generateRandom());
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