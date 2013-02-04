#include "CmCoreGpuObject.h"
#include "CmRenderSystem.h"
#include "CmCoreGpuObjectManager.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	CM_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(mCoreGpuObjectLoadedCondition, CoreGpuObject)
	CM_STATIC_MUTEX_CLASS_INSTANCE(mCoreGpuObjectLoadedMutex, CoreGpuObject)

	CoreGpuObject::CoreGpuObject()
		: mFlags(0), mInternalID(0)
	{
		mInternalID = CoreGpuObjectManager::instance().registerObject(this);
	}

	CoreGpuObject::~CoreGpuObject() 
	{
		if(isInitialized())
		{
			// Object must be released with destroy() otherwise engine can still try to use it, even if it was destructed
			// (e.g. if an object has one of its methods queued in a command queue, and is destructed, you will be accessing invalid memory)
			CM_EXCEPT(InternalErrorException, "Destructor called but object is not destroyed. This will result in nasty issues.");
		}

#if CM_DEBUG_MODE
		if(!mThis.expired())
		{
			CM_EXCEPT(InternalErrorException, "Shared pointer to this object still has active references but " \
				"the object is being deleted? You shouldn't delete CoreGpuObjects manually.");
		}
#endif

		CoreGpuObjectManager::instance().unregisterObject(this);
	}

	void CoreGpuObject::destroy()
	{
		CoreGpuObjectManager::instance().registerObjectToDestroy(mThis.lock());
		RenderSystem::instancePtr()->queueCommand(boost::bind(&CoreGpuObject::destroy_internal, this));
	}

	void CoreGpuObject::destroy_internal()
	{
#if CM_DEBUG_MODE
		if(!isInitialized())
		{
			CoreGpuObjectManager::instance().unregisterObjectToDestroy(mThis.lock());
			CM_EXCEPT(InternalErrorException, "Trying to destroy an object that is already destroyed (or it never was initialized).");
		}
#endif

		setIsInitialized(false);

		CoreGpuObjectManager::instance().unregisterObjectToDestroy(mThis.lock());
	}

	void CoreGpuObject::initialize()
	{
#if CM_DEBUG_MODE
		if(isInitialized())
			CM_EXCEPT(InternalErrorException, "Trying to initialize an object that is already initialized");
#endif

		RenderSystem::instancePtr()->queueCommand(boost::bind(&CoreGpuObject::initialize_internal, this));
	}

	void CoreGpuObject::initialize_internal()
	{
		{
			CM_LOCK_MUTEX(mCoreGpuObjectLoadedMutex);
			setIsInitialized(true);
		}	

		CM_THREAD_NOTIFY_ALL(mCoreGpuObjectLoadedCondition);
	}

	void CoreGpuObject::waitUntilInitialized()
	{
#if CM_DEBUG_MODE
		if(CM_THREAD_CURRENT_ID == RenderSystem::instancePtr()->getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "You cannot call this method on the render thread. It will cause a deadlock!");
#endif

		if(!isInitialized())
		{
			CM_LOCK_MUTEX_NAMED(mCoreGpuObjectLoadedMutex, lock);
			while(!isInitialized())
			{
				CM_THREAD_WAIT(mCoreGpuObjectLoadedCondition, mCoreGpuObjectLoadedMutex, lock);
			}
		}
	}

	void CoreGpuObject::setThisPtr(std::shared_ptr<CoreGpuObject> ptrThis)
	{
		mThis = ptrThis;
	}

	void CoreGpuObject::_deleteDelayed(CoreGpuObject* obj)
	{
		assert(obj != nullptr);

		// This method usually gets called automatically by the shared pointer when all references are released. The process:
		// - If the deletion flag is set or object was never initialized then we immediately delete the object
		// - Otherwise:
		//  - We re-create the reference to the object by setting mThis pointer
		//  - We queue the object to be destroyed so all of its GPU resources may be released on the render thread
		//    - destroy() makes sure it keeps a reference of mThis so object isn't deleted
		//    - Once the destroy() finishes the reference is removed and delete is called again, but this time deletion flag is set

		if(obj->isScheduledToBeDeleted() || !obj->isInitialized())
		{
			delete obj;
		}
		else
		{
			std::shared_ptr<CoreGpuObject> thisPtr(obj);
			obj->setThisPtr(thisPtr);
			obj->setScheduledToBeDeleted(true);
			obj->destroy();
		}
	}
}