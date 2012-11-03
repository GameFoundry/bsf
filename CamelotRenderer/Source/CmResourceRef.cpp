#include "CmPrerequisites.h"
#include "CmResourceRef.h"
#include "CmResource.h"
#include "CmResourceRefRTTI.h"
#include "CmResources.h"

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
	}

	void ResourceRefBase::resolve(std::shared_ptr<Resource> ptr) 
	{ 
		init(ptr);
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
			mData->mIsResolved = true; 
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