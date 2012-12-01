#include "CmPrerequisites.h"
#include "CmResourceHandle.h"
#include "CmResource.h"
#include "CmResourceHandleRTTI.h"
#include "CmResources.h"

namespace CamelotEngine
{
	RTTITypeBase* ResourceHandleData::getRTTIStatic()
	{
		return ResourceHandleDataRTTI::instance();
	}

	RTTITypeBase* ResourceHandleData::getRTTI() const
	{
		return ResourceHandleData::getRTTIStatic();
	}

	ResourceHandleBase::ResourceHandleBase()
	{
		mData = std::shared_ptr<ResourceHandleData>(new ResourceHandleData());
	}

	bool ResourceHandleBase::isLoaded() const 
	{ 
		return (mData->mIsCreated && mData->mPtr != nullptr && mData->mPtr->isInitialized()); 
	}

	void ResourceHandleBase::resolve(std::shared_ptr<Resource> ptr) 
	{ 
		init(ptr);
	}

	void ResourceHandleBase::setUUID(const String& uuid) 
	{ 
		mData->mUUID = uuid;
	}

	void ResourceHandleBase::init(Resource* ptr)
	{
		init(std::shared_ptr<Resource>(ptr));
	}

	void ResourceHandleBase::init(std::shared_ptr<Resource> ptr)
	{
		mData->mPtr = ptr;

		if(mData->mPtr)
		{
			mData->mUUID = mData->mPtr->getUUID();
			mData->mIsCreated = true; 
		}
	}

	RTTITypeBase* ResourceHandleBase::getRTTIStatic()
	{
		return ResourceHandleRTTI::instance();
	}

	RTTITypeBase* ResourceHandleBase::getRTTI() const
	{
		return ResourceHandleBase::getRTTIStatic();
	}
}