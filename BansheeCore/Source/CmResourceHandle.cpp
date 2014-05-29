#include "CmPrerequisites.h"
#include "CmResourceHandle.h"
#include "CmResource.h"
#include "CmResourceHandleRTTI.h"
#include "CmResources.h"

namespace BansheeEngine
{
	BS_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(mResourceCreatedCondition, ResourceHandleBase)
	BS_STATIC_MUTEX_CLASS_INSTANCE(mResourceCreatedMutex, ResourceHandleBase)

	ResourceHandleBase::ResourceHandleBase()
	{
		mData = nullptr;
	}

	bool ResourceHandleBase::isLoaded() const 
	{ 
		return (mData != nullptr && mData->mIsCreated && mData->mPtr != nullptr); 
	}

	void ResourceHandleBase::synchronize() const
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

		mData->mPtr->synchronize();
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