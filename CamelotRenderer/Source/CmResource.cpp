#include "CmResource.h"
#include "CmResourceRTTI.h"
#include "CmUUID.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	CM_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(mResourceLoadedCondition, Resource)
	CM_STATIC_MUTEX_CLASS_INSTANCE(mResourceLoadedMutex, Resource)

	Resource::Resource()
		:mSize(0), mIsInitialized(false)
	{
		// We always generate a random UUID, and then overwrite it with the actual one 
		// during loading if one was previously generated and saved.
		mUUID = UUIDGenerator::generateRandom();
	}

	void Resource::initialize_internal()
	{
		{
			CM_LOCK_MUTEX(mResourceLoadedMutex);
			mIsInitialized = true;
		}	

		CM_THREAD_NOTIFY_ALL(mResourceLoadedCondition);
	}

	void Resource::destroy_internal()
	{
		CoreGpuObject::destroy_internal();
	}

	void Resource::waitUntilInitialized()
	{
#if CM_DEBUG_MODE
		if(CM_THREAD_CURRENT_ID == RenderSystem::instancePtr()->getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "You cannot call this method on the render thread. It will cause a deadlock!");
#endif

		if(!mIsInitialized)
		{
			CM_LOCK_MUTEX_NAMED(mResourceLoadedMutex, lock);
			while(!mIsInitialized)
			{
				CM_THREAD_WAIT(mResourceLoadedCondition, mResourceLoadedMutex, lock);
			}
		}
	}
		
	RTTITypeBase* Resource::getRTTIStatic()
	{
		return ResourceRTTI::instance();
	}

	RTTITypeBase* Resource::getRTTI() const
	{
		return Resource::getRTTIStatic();
	}
}