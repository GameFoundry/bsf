#include "BsCoreObject.h"
#include "BsCoreObjectCore.h"
#include "BsCoreThread.h"
#include "BsCoreObjectManager.h"
#include "BsCoreThreadAccessor.h"
#include "BsFrameAlloc.h"
#include "BsDebug.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	CoreObject::CoreObject(bool initializeOnRenderThread)
		: mFlags(0), mInternalID(0), mCoreDirtyFlags(0xFFFFFFFF)
	{
		mInternalID = CoreObjectManager::instance().registerObject(this);
		mFlags = initializeOnRenderThread ? mFlags | CGO_INIT_ON_CORE_THREAD : mFlags;
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

		CoreObjectManager::instance().unregisterObject(this);
	}

	void CoreObject::destroy()
	{
		setIsDestroyed(true);

		if(requiresInitOnCoreThread())
		{
			assert(BS_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId() && "Cannot destroy sim thead object from core thread.");

			queueDestroyGpuCommand(mCoreSpecific);
		}

		mCoreSpecific = nullptr;
	}

	void CoreObject::initialize()
	{
		mCoreSpecific = createCore();

		if (mCoreSpecific != nullptr)
		{
			if (requiresInitOnCoreThread())
			{
				mCoreSpecific->setScheduledToBeInitialized(true);

				assert(BS_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId() && "Cannot initialize sim thread object from core thread.");

				queueInitializeGpuCommand(mCoreSpecific);
			}
			else
			{
				mCoreSpecific->initialize();
			}
		}
	}

	void CoreObject::blockUntilCoreInitialized()
	{
		if (mCoreSpecific != nullptr)
			mCoreSpecific->synchronize();
	}

	void CoreObject::syncToCore(CoreAccessor& accessor)
	{
		struct IndividualCoreSyncData
		{
			SPtr<CoreObjectCore> destination;
			CoreSyncData syncData;
			FrameAlloc* allocator;
		};

		FrameAlloc* allocator = gCoreThread().getFrameAlloc();

		Vector<SPtr<CoreObject>> dependencies;
		Vector<IndividualCoreSyncData> syncData;

		UINT32 stackPos = 0;

		dependencies.push_back(getThisPtr());
		while (stackPos < dependencies.size())
		{
			SPtr<CoreObject> curObj = dependencies[stackPos];
			stackPos++;

			if (curObj->isCoreDirty())
			{
				SPtr<CoreObjectCore> destObj = curObj->getCore();
				if (destObj == nullptr)
					return;

				IndividualCoreSyncData data;
				data.allocator = allocator;
				data.destination = destObj;
				data.syncData = syncToCore(data.allocator);

				syncData.push_back(data);

				curObj->markCoreClean();
			}

			// Note: I don't check for recursion. Possible infinite loop if two objects
			// are dependent on one another.
			curObj->getCoreDependencies(dependencies);
		}

		std::function<void(const Vector<IndividualCoreSyncData>&)> callback =
			[](const Vector<IndividualCoreSyncData>& data)
		{
			// Traverse in reverse to sync dependencies before dependants
			for (auto& riter = data.rbegin(); riter != data.rend(); ++riter)
			{
				const IndividualCoreSyncData& entry = *riter;
				entry.destination->syncToCore(entry.syncData);

				UINT8* dataPtr = entry.syncData.getBuffer();

				if (dataPtr != nullptr)
					entry.allocator->dealloc(dataPtr);
			}
		};

		if (syncData.size() > 0)
			accessor.queueCommand(std::bind(callback, syncData));
	}

	void CoreObject::_setThisPtr(std::shared_ptr<CoreObject> ptrThis)
	{
		mThis = ptrThis;
	}

	void CoreObject::queueGpuCommand(const SPtr<CoreObjectCore>& obj, std::function<void()> func)
	{
		// We call another internal method and go through an additional layer of abstraction in order to keep an active
		// reference to the obj (saved in the bound function).
		// We could have called the function directly using "this" pointer but then we couldn't have used a shared_ptr for the object,
		// in which case there is a possibility that the object would be released and deleted while still being in the command queue.
		gCoreAccessor().queueCommand(std::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	AsyncOp CoreObject::queueReturnGpuCommand(const SPtr<CoreObjectCore>& obj, std::function<void(AsyncOp&)> func)
	{
		// See queueGpuCommand
		return gCoreAccessor().queueReturnCommand(std::bind(&CoreObject::executeReturnGpuCommand, obj, func, _1));
	}

	void CoreObject::queueInitializeGpuCommand(const SPtr<CoreObjectCore>& obj)
	{
		std::function<void()> func = std::bind(&CoreObjectCore::initialize, obj.get());

		CoreThread::instance().queueCommand(std::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	void CoreObject::queueDestroyGpuCommand(const SPtr<CoreObjectCore>& obj)
	{
		std::function<void()> func = [&](){}; // Do nothing function. We just need the shared pointer to stay alive until it reaches the core thread

		gCoreAccessor().queueCommand(std::bind(&CoreObject::executeGpuCommand, obj, func));
	}

	void CoreObject::executeGpuCommand(const SPtr<CoreObjectCore>& obj, std::function<void()> func)
	{
		volatile std::shared_ptr<CoreObjectCore> objParam = obj; // Makes sure obj isn't optimized out?

		func();
	}

	void CoreObject::executeReturnGpuCommand(const SPtr<CoreObjectCore>& obj, std::function<void(AsyncOp&)> func, AsyncOp& op)
	{
		volatile std::shared_ptr<CoreObjectCore> objParam = obj; // Makes sure obj isn't optimized out?

		func(op);
	}
}