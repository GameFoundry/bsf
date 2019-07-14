//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "CoreThread/BsCoreObject.h"
#include "CoreThread/BsCoreObjectCore.h"
#include "CoreThread/BsCoreThread.h"
#include "CoreThread/BsCoreObjectManager.h"

using namespace std::placeholders;

namespace bs
{
	CoreObject::CoreObject(bool initializeOnCoreThread)
		: mFlags(initializeOnCoreThread ? CGO_INIT_ON_CORE_THREAD : 0)
		, mCoreDirtyFlags(0)
		, mInternalID(CoreObjectManager::instance().generateId())
	{
	}

	CoreObject::~CoreObject()
	{
		if(!isDestroyed())
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
	}

	void CoreObject::destroy()
	{
		CoreObjectManager::instance().unregisterObject(this);
		setIsDestroyed(true);

		if(requiresInitOnCoreThread())
		{
#if !BS_FORCE_SINGLETHREADED_RENDERING
			assert(BS_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId() && "Cannot destroy sim thead object from core thread.");
#endif

			// This will only destroy the ct::CoreObject if this was the last reference
			queueDestroyGpuCommand(mCoreSpecific);
		}

		mCoreSpecific = nullptr;
	}

	void CoreObject::initialize()
	{
		CoreObjectManager::instance().registerObject(this);
		mCoreSpecific = createCore();

		if (mCoreSpecific != nullptr)
		{
			if (requiresInitOnCoreThread())
			{
				mCoreSpecific->setScheduledToBeInitialized(true);

#if !BS_FORCE_SINGLETHREADED_RENDERING
				assert(BS_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId() && "Cannot initialize sim thread object from core thread.");
#endif

				queueInitializeGpuCommand(mCoreSpecific);
			}
			else
			{
				mCoreSpecific->initialize();

				// Even though this object might not require initialization on the core thread, it will be used on it, therefore
				// do a memory barrier to ensure any stores are finished before continuing (When it requires init on core thread
				// we use the core queue which uses a mutex, and therefore executes all stores as well, so we dont need to
				// do this explicitly)
				std::atomic_thread_fence(std::memory_order_release); // TODO - Need atomic variable, currently this does nothing
			}
		}

		mFlags |= CGO_INITIALIZED;
		markDependenciesDirty();
	}

	void CoreObject::blockUntilCoreInitialized() const
	{
		if (mCoreSpecific != nullptr)
			mCoreSpecific->synchronize();
	}

	void CoreObject::syncToCore()
	{
		CoreObjectManager::instance().syncToCore(this);
	}

	void CoreObject::markCoreDirty(UINT32 flags)
	{
		bool wasDirty = isCoreDirty();

		mCoreDirtyFlags |= flags;

		if (!wasDirty && isCoreDirty())
			CoreObjectManager::instance().notifyCoreDirty(this);
	}

	void CoreObject::markDependenciesDirty()
	{
		CoreObjectManager::instance().notifyDependenciesDirty(this);
	}

	void CoreObject::_setThisPtr(SPtr<CoreObject> ptrThis)
	{
		mThis = ptrThis;
	}

	void CoreObject::queueGpuCommand(const SPtr<ct::CoreObject>& obj, std::function<void()> func)
	{
		// We call another internal method and go through an additional layer of abstraction in order to keep an active
		// reference to the obj (saved in the bound function).
		// We could have called the function directly using "this" pointer but then we couldn't have used a shared_ptr for the object,
		// in which case there is a possibility that the object would be released and deleted while still being in the command queue.
		gCoreThread().queueCommand(std::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	AsyncOp CoreObject::queueReturnGpuCommand(const SPtr<ct::CoreObject>& obj, std::function<void(AsyncOp&)> func)
	{
		// See queueGpuCommand
		return gCoreThread().queueReturnCommand(std::bind(&CoreObject::executeReturnGpuCommand, obj, func, _1));
	}

	void CoreObject::queueInitializeGpuCommand(const SPtr<ct::CoreObject>& obj)
	{
		std::function<void()> func = std::bind(&ct::CoreObject::initialize, obj.get());

		CoreThread::instance().queueCommand(std::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	void CoreObject::queueDestroyGpuCommand(const SPtr<ct::CoreObject>& obj)
	{
		std::function<void()> func = [&](){}; // Do nothing function. We just need the shared pointer to stay alive until it reaches the core thread

		gCoreThread().queueCommand(std::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	void CoreObject::executeGpuCommand(const SPtr<ct::CoreObject>& obj, std::function<void()> func)
	{
		volatile SPtr<ct::CoreObject> objParam = obj; // Makes sure obj isn't optimized out?

		func();
	}

	void CoreObject::executeReturnGpuCommand(const SPtr<ct::CoreObject>& obj, std::function<void(AsyncOp&)> func,
		AsyncOp& op)
	{
		volatile SPtr<ct::CoreObject> objParam = obj; // Makes sure obj isn't optimized out?

		func(op);
	}
}
