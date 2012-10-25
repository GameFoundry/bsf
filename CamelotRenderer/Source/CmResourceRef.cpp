#include "CmPrerequisites.h"
#include "CmResourceRef.h"
#include "CmResource.h"

namespace CamelotEngine
{
	void ResourceRefBase::init(Resource* ptr)
	{
		init(std::shared_ptr<Resource>(ptr));
	}

	void ResourceRefBase::init(std::shared_ptr<Resource> ptr)
	{
		mPtr = ptr;

		if(mPtr)
		{
			mUUID = mPtr->getUUID();
			mUUIDSet = true;
		}
	}
}