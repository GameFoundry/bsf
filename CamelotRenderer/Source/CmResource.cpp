#include "CmResource.h"
#include "CmResourceRTTI.h"

namespace CamelotEngine
{
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