#include "BsCoreObject.h"
#include "BsCoreThread.h"
#include "BsCoreObjectManager.h"
#include "BsCoreThreadAccessor.h"
#include "BsDebug.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	BS_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(mCoreGpuObjectLoadedCondition, CoreObject)
	BS_STATIC_MUTEX_CLASS_INSTANCE(mCoreGpuObjectLoadedMutex, CoreObject)

	CoreObject::CoreObject(bool initializeOnRenderThread)
		: mFlags(0), mInternalID(0), mCoreDirtyFlags(0xFFFFFFFF), mCoreSpecific(nullptr)
	{
		mInternalID = CoreObjectManager::instance().registerObject(this);
		mFlags = initializeOnRenderThread ? mFlags | CGO_INIT_ON_CORE_THREAD : mFlags;
	}

	CoreObject::~CoreObject() 
	{
		if(isInitialized())
		{
			// Object must be released with destroy() otherwise engine can still try to use it, even if it was destructed
			// (e.g. if an object has one of its methods queued in a command queue, and is destructed, you will be accessing invalid memory)
			BS_EXCEPT(InternalErrorException, "Destructor called but object is not destroyed. This will result in nasty issues.");
		}

#if BS_DEBUG_MODE
		if(!mThis.expired())
		{
			BS_EXCEPT(InternalErrorException, "Shared pointer to this object still has active references but " \
				"the object is being deleted? You shouldn't delete CoreObjects manually.");
		}
#endif

		if (mCoreSpecific != nullptr)
		{
			bs_delete(mCoreSpecific);
			mCoreSpecific = nullptr;
		}

		CoreObjectManager::instance().unregisterObject(this);
	}

	void CoreObject::destroy()
	{
		if(requiresInitOnCoreThread())
		{
			setScheduledToBeDeleted(true);

			if(BS_THREAD_CURRENT_ID == CoreThread::instance().getCoreThreadId())
				mThis.lock()->destroy_internal();
			else
				queueDestroyGpuCommand(mThis.lock());
		}
		else
		{
			destroy_internal();
		}
	}

	void CoreObject::destroy_internal()
	{
#if BS_DEBUG_MODE
		if(!isInitialized())
			BS_EXCEPT(InternalErrorException, "Trying to destroy an object that is already destroyed (or it never was initialized).");
#endif

		if (mCoreSpecific != nullptr)
			mCoreSpecific->destroy();

		setIsInitialized(false);
	}

	void CoreObject::initialize()
	{
#if BS_DEBUG_MODE
		if(isInitialized() || isScheduledToBeInitialized())
			BS_EXCEPT(InternalErrorException, "Trying to initialize an object that is already initialized.");
#endif

		mCoreSpecific = createCore();

		if(requiresInitOnCoreThread())
		{
			setScheduledToBeInitialized(true);

			if(BS_THREAD_CURRENT_ID == CoreThread::instance().getCoreThreadId())
				mThis.lock()->initialize_internal();
			else
				queueInitializeGpuCommand(mThis.lock());
		}
		else
		{
			initialize_internal();
		}
	}

	void CoreObject::initialize_internal()
	{
		if (mCoreSpecific != nullptr)
			mCoreSpecific->initialize();

		if(requiresInitOnCoreThread())
		{
			{
				BS_LOCK_MUTEX(mCoreGpuObjectLoadedMutex);
				setIsInitialized(true);
			}	

			setScheduledToBeInitialized(false);

			BS_THREAD_NOTIFY_ALL(mCoreGpuObjectLoadedCondition);
		}
		else
		{
			setIsInitialized(true);
		}
	}

	void CoreObject::synchronize()
	{
		if(!isInitialized())
		{
			if(requiresInitOnCoreThread())
			{
#if BS_DEBUG_MODE
				if(BS_THREAD_CURRENT_ID == CoreThread::instance().getCoreThreadId())
					BS_EXCEPT(InternalErrorException, "You cannot call this method on the core thread. It will cause a deadlock!");
#endif

				BS_LOCK_MUTEX_NAMED(mCoreGpuObjectLoadedMutex, lock);
				while(!isInitialized())
				{
					if(!isScheduledToBeInitialized())
						BS_EXCEPT(InternalErrorException, "Attempting to wait until initialization finishes but object is not scheduled to be initialized.");

					BS_THREAD_WAIT(mCoreGpuObjectLoadedCondition, mCoreGpuObjectLoadedMutex, lock);
				}
			}
			else
			{
				BS_EXCEPT(InternalErrorException, "Attempting to wait until initialization finishes but object is not scheduled to be initialized.");
			}
		}
	}

	void CoreObject::_setThisPtr(std::shared_ptr<CoreObject> ptrThis)
	{
		mThis = ptrThis;
	}

	void CoreObject::_deleteDelayedInternal(CoreObject* obj)
	{
		assert(obj != nullptr);

		// This method usually gets called automatically by the shared pointer when all references are released. The process:
		// - If the object wasn't initialized delete it right away
		// - Otherwise:
		//  - We re-create the reference to the object by setting mThis pointer
		//  - We queue the object to be destroyed so all of its GPU resources may be released on the core thread
		//    - destroy() makes sure it keeps a reference of mThis so object isn't deleted
		//    - Once the destroy() finishes the reference is removed and the default shared_ptr deleter is called

#if BS_DEBUG_MODE
		if(obj->isScheduledToBeInitialized())
		{
			BS_EXCEPT(InternalErrorException, "Object scheduled to be initialized, yet it's being deleted. " \
				"By design objects queued in the command queue should always have a reference count >= 1, therefore never be deleted " \
				"while still in the queue.");
		}
#endif
	}

	void CoreObject::queueGpuCommand(std::shared_ptr<CoreObject>& obj, std::function<void()> func)
	{
		// We call another internal method and go through an additional layer of abstraction in order to keep an active
		// reference to the obj (saved in the bound function).
		// We could have called the function directly using "this" pointer but then we couldn't have used a shared_ptr for the object,
		// in which case there is a possibility that the object would be released and deleted while still being in the command queue.
		gCoreAccessor().queueCommand(std::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	AsyncOp CoreObject::queueReturnGpuCommand(std::shared_ptr<CoreObject>& obj, std::function<void(AsyncOp&)> func)
	{
		// See queueGpuCommand
		return gCoreAccessor().queueReturnCommand(std::bind(&CoreObject::executeReturnGpuCommand, obj, func, _1));
	}

	void CoreObject::queueInitializeGpuCommand(std::shared_ptr<CoreObject>& obj)
	{
		std::function<void()> func = std::bind(&CoreObject::initialize_internal, obj.get());

		CoreThread::instance().queueCommand(std::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	void CoreObject::queueDestroyGpuCommand(std::shared_ptr<CoreObject>& obj)
	{
		std::function<void()> func = std::bind(&CoreObject::destroy_internal, obj.get());

		gCoreAccessor().queueCommand(std::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	void CoreObject::executeGpuCommand(std::shared_ptr<CoreObject> obj, std::function<void()> func)
	{
		volatile std::shared_ptr<CoreObject> objParam = obj; // Makes sure obj isn't optimized out?

		func();
	}

	void CoreObject::executeReturnGpuCommand(std::shared_ptr<CoreObject> obj, std::function<void(AsyncOp&)> func, AsyncOp& op)
	{
		volatile std::shared_ptr<CoreObject> objParam = obj; // Makes sure obj isn't optimized out?

		func(op);
	}
}