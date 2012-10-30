#include "CmPrerequisites.h"
#include "CmResourceRef.h"
#include "CmResource.h"
#include "CmResourceRefRTTI.h"

namespace CamelotEngine
{
	RTTITypeBase* ResourceRefData::getRTTIStatic()
	{
		return ResourceRefDataRTTI::instance();
	}

	RTTITypeBase* ResourceRefData::getRTTI() const
	{
		return ResourceRefData::getRTTIStatic();
	}

	ResourceRefBase::ResourceRefBase()
	{
		mData = std::shared_ptr<ResourceRefData>(new ResourceRefData());
		mData->mUUIDSet = false;
	}

	void ResourceRefBase::init(Resource* ptr)
	{
		init(std::shared_ptr<Resource>(ptr));
	}

	void ResourceRefBase::init(std::shared_ptr<Resource> ptr)
	{
		mData->mPtr = ptr;

		if(mData->mPtr)
		{
			mData->mUUID = mData->mPtr->getUUID();
			mData->mUUIDSet = true;
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