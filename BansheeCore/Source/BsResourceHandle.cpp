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

		if (!mData->mIsCreated)
		{
			BS_LOCK_MUTEX_NAMED(mResourceCreatedMutex, lock);
			while (!mData->mIsCreated)
			{
				BS_THREAD_WAIT(mResourceCreatedCondition, mResourceCreatedMutex, lock);
			}

			// Send out ResourceListener events right away, as whatever called this method
			// probably also expects the listener events to trigger immediately as well
			ResourceListenerManager::instance().notifyListeners(mData->mUUID);
		}

		if (waitForDependencies)
		{
			bs_frame_mark();

			{
				FrameVector<HResource> dependencies;
				mData->mPtr->getResourceDependencies(dependencies);

				for (auto& dependency : dependencies)
					dependency.blockUntilLoaded(waitForDependencies);
			}

			bs_frame_clear();
		}
	}

	void ResourceHandleBase::release()
	{
		gResources().release(*this);
	}

	void ResourceHandleBase::destroy()
	{
		gResources().destroy(*this);
	}

	void ResourceHandleBase::setHandleData(const SPtr<Resource>& ptr, const String& uuid)
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

	void ResourceHandleBase::addInternalRef()
	{
		mData->mRefCount++;
	}

	void ResourceHandleBase::removeInternalRef()
	{
		mData->mRefCount--;
	}

	void ResourceHandleBase::throwIfNotLoaded() const
	{
#if BS_DEBUG_MODE
		if (!isLoaded(false))
		{
			BS_EXCEPT(InternalErrorException, "Trying to access a resource that hasn't been loaded yet.");
		}
#endif
	}

	template class TResourceHandleBase<true>;
	template class TResourceHandleBase<false>;

	RTTITypeBase* TResourceHandleBase<true>::getRTTIStatic()
	{
		return WeakResourceHandleRTTI::instance();
	}

	RTTITypeBase* TResourceHandleBase<true>::getRTTI() const
	{
		return getRTTIStatic();
	}

	RTTITypeBase* TResourceHandleBase<false>::getRTTIStatic()
	{
		return ResourceHandleRTTI::instance();
	}

	RTTITypeBase* TResourceHandleBase<false>::getRTTI() const
	{
		return getRTTIStatic();
	}
}