#include "BsCorePrerequisites.h"
#include "BsResourceHandle.h"
#include "BsResource.h"
#include "BsResourceHandleRTTI.h"
#include "BsResources.h"
#include "BsResourceListenerManager.h"

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

	bool ResourceHandleBase::isLoaded(bool checkDependencies) const 
	{ 
		bool isLoaded = (mData != nullptr && mData->mIsCreated && mData->mPtr != nullptr);

		if (checkDependencies && isLoaded)
			isLoaded = mData->mPtr->areDependenciesLoaded();

		return isLoaded;
	}

	void ResourceHandleBase::blockUntilLoaded(bool waitForDependencies) const
	{
		if(mData == nullptr)
			return;

		if (!isLoaded(waitForDependencies))
		{
			BS_LOCK_MUTEX_NAMED(mResourceCreatedMutex, lock);
			while (!mData->mIsCreated || (waitForDependencies && !mData->mPtr->areDependenciesLoaded()))
			{
				BS_THREAD_WAIT(mResourceCreatedCondition, mResourceCreatedMutex, lock);
			}

			// Send out ResourceListener events right away, as whatever called this method
			// probably also expects the listener events to trigger immediately as well
			ResourceListenerManager::instance().notifyListeners(mData->mUUID);
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