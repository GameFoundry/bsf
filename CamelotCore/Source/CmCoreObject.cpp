#include "CmCoreObject.h"
#include "CmRenderSystem.h"
#include "CmCoreObjectManager.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	CM_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(mCoreGpuObjectLoadedCondition, CoreObject)
	CM_STATIC_MUTEX_CLASS_INSTANCE(mCoreGpuObjectLoadedMutex, CoreObject)

	CoreObject::CoreObject(bool initializeOnRenderThread)
		: mFlags(0), mInternalID(0)
	{
		mInternalID = CoreGpuObjectManager::instance().registerObject(this);
		mFlags = initializeOnRenderThread ? mFlags | CGO_INIT_ON_RENDER_THREAD : mFlags;
	}

	CoreObject::~CoreObject() 
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

	void CoreObject::destroy()
	{
		if(requiresInitOnRenderThread())
		{
			setScheduledToBeDeleted(true);

			queueGpuCommand(mThis.lock(), boost::bind(&CoreObject::destroy_internal, this));
		}
		else
		{
			destroy_internal();
		}
	}

	void CoreObject::destroy_internal()
	{
#if CM_DEBUG_MODE
		if(!isInitialized())
			CM_EXCEPT(InternalErrorException, "Trying to destroy an object that is already destroyed (or it never was initialized).");
#endif

		setIsInitialized(false);
	}

	void CoreObject::initialize()
	{
#if CM_DEBUG_MODE
		if(isInitialized() || isScheduledToBeInitialized())
			CM_EXCEPT(InternalErrorException, "Trying to initialize an object that is already initialized.");
#endif

		if(requiresInitOnRenderThread())
		{
			setScheduledToBeInitialized(true);

			queueGpuCommand(mThis.lock(), boost::bind(&CoreObject::initialize_internal, this));
		}
		else
		{
			initialize_internal();
		}
	}

	void CoreObject::initialize_internal()
	{
		if(requiresInitOnRenderThread())
		{
			{
				CM_LOCK_MUTEX(mCoreGpuObjectLoadedMutex);
				setIsInitialized(true);
			}	

			setScheduledToBeInitialized(false);

			CM_THREAD_NOTIFY_ALL(mCoreGpuObjectLoadedCondition);
		}
		else
		{
			setIsInitialized(true);
		}
	}

	void CoreObject::waitUntilInitialized()
	{
#if CM_DEBUG_MODE
		if(CM_THREAD_CURRENT_ID == RenderSystem::instancePtr()->getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "You cannot call this method on the render thread. It will cause a deadlock!");
#endif

		if(!isInitialized())
		{
			if(requiresInitOnRenderThread())
			{
				CM_LOCK_MUTEX_NAMED(mCoreGpuObjectLoadedMutex, lock);
				while(!isInitialized())
				{
					if(!isScheduledToBeInitialized())
						CM_EXCEPT(InternalErrorException, "Attempting to wait until initialization finishes but object is not scheduled to be initialized.");

					CM_THREAD_WAIT(mCoreGpuObjectLoadedCondition, mCoreGpuObjectLoadedMutex, lock);
				}
			}
			else
			{
				CM_EXCEPT(InternalErrorException, "Attempting to wait until initialization finishes but object is not scheduled to be initialized.");
			}
		}
	}

	void CoreObject::setThisPtr(std::shared_ptr<CoreObject> ptrThis)
	{
		mThis = ptrThis;
	}

	void CoreObject::_deleteDelayed(CoreObject* obj)
	{
		assert(obj != nullptr);

		// This method usually gets called automatically by the shared pointer when all references are released. The process:
		// - If the object wasn't initialized delete it right away
		// - Otherwise:
		//  - We re-create the reference to the object by setting mThis pointer
		//  - We queue the object to be destroyed so all of its GPU resources may be released on the render thread
		//    - destroy() makes sure it keeps a reference of mThis so object isn't deleted
		//    - Once the destroy() finishes the reference is removed and the default shared_ptr deleter is called

#if CM_DEBUG_MODE
		if(obj->isScheduledToBeInitialized())
		{
			CM_EXCEPT(InternalErrorException, "Object scheduled to be initialized, yet it's being deleted. " \
				"By design objects queued in the command queue should always have a reference count >= 1, therefore never be deleted " \
				"while still in the queue.");
		}
#endif

		if(obj->isInitialized())
		{
			std::shared_ptr<CoreObject> thisPtr(obj);
			obj->setThisPtr(thisPtr);
			obj->destroy();
		}
		else
		{
			delete obj;
		}
	}

	void CoreObject::queueGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void()> func)
	{
		// We call another internal method and go through an additional layer of abstraction in order to keep an active
		// reference to the obj (saved in the bound function).
		// We could have called the function directly using "this" pointer but then we couldn't have used a shared_ptr for the object,
		// in which case there is a possibility that the object would be released and deleted while still being in the command queue.
		RenderSystem::instancePtr()->queueCommand(boost::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	AsyncOp CoreObject::queueReturnGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void(AsyncOp&)> func)
	{
		// See queueGpuCommand
		return RenderSystem::instancePtr()->queueReturnCommand(boost::bind(&CoreObject::executeReturnGpuCommand, obj, func, _1));
	}

	void CoreObject::executeGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void()> func)
	{
		volatile std::shared_ptr<CoreObject> objParam = obj; // Makes sure obj isn't optimized out?

		func();
	}

	void CoreObject::executeReturnGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void(AsyncOp&)> func, AsyncOp& op)
	{
		volatile std::shared_ptr<CoreObject> objParam = obj; // Makes sure obj isn't optimized out?

		func(op);
	}
}