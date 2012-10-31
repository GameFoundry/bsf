#include "CmResource.h"
#include "CmResourceRTTI.h"
#include "CmUUID.h"

namespace CamelotEngine
{
	Resource::Resource()
		:mSize(0), mLoadState(RS_Unloaded)
	{
		// We always generate a random UUID, and then overwrite it with the actual one 
		// during loading if one was previously generated and saved.
		mUUID = UUIDGenerator::generateRandom();
	}

	void Resource::load()
	{
		if(mLoadState != RS_Loaded)
		{
			loadImpl();

			mLoadState = RS_Loaded;
		}
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