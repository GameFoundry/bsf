#include "CmPrerequisites.h"
#include "CmResourceRef.h"
#include "CmResource.h"
#include "CmResourceRefRTTI.h"

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

	RTTITypeBase* ResourceRefBase::getRTTIStatic()
	{
		return ResourceRefRTTI::instance();
	}

	RTTITypeBase* ResourceRefBase::getRTTI() const
	{
		return ResourceRefBase::getRTTIStatic();
	}
}