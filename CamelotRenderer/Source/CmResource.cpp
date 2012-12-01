#include "CmResource.h"
#include "CmResourceRTTI.h"
#include "CmUUID.h"

namespace CamelotEngine
{
	Resource::Resource()
		:mSize(0), mIsInitialized(false)
	{
		// We always generate a random UUID, and then overwrite it with the actual one 
		// during loading if one was previously generated and saved.
		mUUID = UUIDGenerator::generateRandom();
	}

	void Resource::initialize_internal()
	{
		mIsInitialized = true;
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