#include "CmResource.h"
#include "CmResourceRTTI.h"
#include "CmUUID.h"
#include "CmRenderSystem.h"

namespace CamelotFramework
{
	Resource::Resource(bool initializeOnRenderThread)
		:CoreObject(initializeOnRenderThread), mSize(0)
	{
		// We always generate a random UUID, and then overwrite it with the actual one 
		// during loading if one was previously generated and saved.
		mUUID = UUIDGenerator::generateRandom();
	}

	HResource Resource::_createResourceHandle(ResourcePtr obj)
	{
		return HResource(obj);
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