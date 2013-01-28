#include "CmCoreGpuObject.h"
#include "CmRenderSystem.h"
#include "CmCoreGpuObjectManager.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	CM_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(mCoreGpuObjectLoadedCondition, CoreGpuObject)
	CM_STATIC_MUTEX_CLASS_INSTANCE(mCoreGpuObjectLoadedMutex, CoreGpuObject)

	CoreGpuObject::CoreGpuObject()
		: mIsInitialized(false), mInternalID(0)
	{
		mInternalID = CoreGpuObjectManager::instance().registerObject(this);
	}

	CoreGpuObject::~CoreGpuObject() 
	{
		if(mIsInitialized)
		{
			LOGWRN("Destructor called but object is not destroyed. Object will leak.")
		}

		CoreGpuObjectManager::instance().unregisterObject(this);
	}

	void CoreGpuObject::destroy()
	{
#if CM_DEBUG_MODE
		if(!mIsInitialized)
			CM_EXCEPT(InternalErrorException, "Trying to destroy an object that is already destroyed (or it never was initialized).");
#endif

		CoreGpuObjectManager::instance().registerObjectToDestroy(mThis.lock());
		RenderSystem::instancePtr()->queueCommand(boost::bind(&CoreGpuObject::destroy_internal, this));
	}

	void CoreGpuObject::destroy_internal()
	{
		mIsInitialized = false;

		CoreGpuObjectManager::instance().unregisterObjectToDestroy(mThis.lock());
	}

	void CoreGpuObject::initialize()
	{
#if CM_DEBUG_MODE
		if(mIsInitialized)
			CM_EXCEPT(InternalErrorException, "Trying to initialize an object that is already initialized");
#endif

		RenderSystem::instancePtr()->queueCommand(boost::bind(&CoreGpuObject::initialize_internal, this));
	}

	void CoreGpuObject::initialize_internal()
	{
		{
			CM_LOCK_MUTEX(mCoreGpuObjectLoadedMutex);
			mIsInitialized = true;
		}	

		CM_THREAD_NOTIFY_ALL(mCoreGpuObjectLoadedCondition);
	}

	void CoreGpuObject::waitUntilInitialized()
	{
#if CM_DEBUG_MODE
		if(CM_THREAD_CURRENT_ID == RenderSystem::instancePtr()->getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "You cannot call this method on the render thread. It will cause a deadlock!");
#endif

		if(!mIsInitialized)
		{
			CM_LOCK_MUTEX_NAMED(mCoreGpuObjectLoadedMutex, lock);
			while(!mIsInitialized)
			{
				CM_THREAD_WAIT(mCoreGpuObjectLoadedCondition, mCoreGpuObjectLoadedMutex, lock);
			}
		}
	}

	void CoreGpuObject::setThisPtr(std::shared_ptr<CoreGpuObject> ptrThis)
	{
		mThis = ptrThis;
	}
}