#include "CmPrerequisites.h"
#include "CmResourceHandle.h"
#include "CmResource.h"
#include "CmResourceHandleRTTI.h"
#include "CmResources.h"

namespace CamelotFramework
{
	CM_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(mResourceCreatedCondition, ResourceHandleBase)
	CM_STATIC_MUTEX_CLASS_INSTANCE(mResourceCreatedMutex, ResourceHandleBase)

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
			CM_LOCK_MUTEX_NAMED(mResourceCreatedMutex, lock);
			while(!mData->mIsCreated)
			{
				CM_THREAD_WAIT(mResourceCreatedCondition, mResourceCreatedMutex, lock);
			}
		}

		mData->mPtr->synchonize();
	}

	void ResourceHandleBase::setResourcePtr(std::shared_ptr<Resource> ptr)
	{
		mData->mPtr = ptr;

		if(mData->mPtr)
		{
			mData->mUUID = mData->mPtr->getUUID();
		
			if(!mData->mIsCreated)
			{
				CM_LOCK_MUTEX(mResourceCreatedMutex);
				{
					mData->mIsCreated = true; 
				}
				
				CM_THREAD_NOTIFY_ALL(mResourceCreatedCondition);
			}
		}
	}

	void ResourceHandleBase::throwIfNotLoaded() const
	{
		if(!isLoaded()) 
		{
			CM_EXCEPT(InternalErrorException, "Trying to access a resource that hasn't been loaded yet.");
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