#include "BsCorePrerequisites.h"
#include "BsResourceHandle.h"
#include "BsResource.h"
#include "BsResourceHandleRTTI.h"
#include "BsResources.h"

namespace BansheeEngine
{
	BS_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(mResourceCreatedCondition, ResourceHandleBase)
	BS_STATIC_MUTEX_CLASS_INSTANCE(mResourceCreatedMutex, ResourceHandleBase)

	ResourceHandleBase::ResourceHandleBase()
	{
		mData = nullptr;
	}

	ResourceHandleBase::~ResourceHandleBase() 
	{ 

	}

	bool ResourceHandleBase::isLoaded() const 
	{ 
		return (mData != nullptr && mData->mIsCreated && mData->mPtr != nullptr); 
	}

	void ResourceHandleBase::blockUntilLoaded() const
	{
		if(mData == nullptr)
			return;

		if(!mData->mIsCreated)
		{
			BS_LOCK_MUTEX_NAMED(mResourceCreatedMutex, lock);
			while(!mData->mIsCreated)
			{
				BS_THREAD_WAIT(mResourceCreatedCondition, mResourceCreatedMutex, lock);
			}
		}
	}

	void ResourceHandleBase::_setHandleData(std::shared_ptr<Resource> ptr, const String& uuid)
	{
		mData->mPtr = ptr;

		if(mData->mPtr)
		{
			mData->mUUID = uuid;
		
			if(!mData->mIsCreated)
			{
				BS_LOCK_MUTEX(mResourceCreatedMutex);
				{
					mData->mIsCreated = true; 
				}
				
				BS_THREAD_NOTIFY_ALL(mResourceCreatedCondition);
			}
		}
	}

	void ResourceHandleBase::_setHandleData(const std::shared_ptr<ResourceHandleData>& data)
	{
		mData = data;
	}

	void ResourceHandleBase::throwIfNotLoaded() const
	{
		if(!isLoaded()) 
		{
			BS_EXCEPT(InternalErrorException, "Trying to access a resource that hasn't been loaded yet.");
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