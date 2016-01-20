//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCoreObjectCore.h"
#include "BsCoreThread.h"

namespace BansheeEngine
{
	BS_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(mCoreGpuObjectLoadedCondition, CoreObjectCore)
		BS_STATIC_MUTEX_CLASS_INSTANCE(mCoreGpuObjectLoadedMutex, CoreObjectCore)

	CoreObjectCore::CoreObjectCore()
		:mFlags(0)
	{ }

	CoreObjectCore::~CoreObjectCore()
	{ 
		THROW_IF_NOT_CORE_THREAD;
	}

	void CoreObjectCore::initialize()
	{
		{
			BS_LOCK_MUTEX(mCoreGpuObjectLoadedMutex);
			setIsInitialized(true);
		}

		setScheduledToBeInitialized(false);

		BS_THREAD_NOTIFY_ALL(mCoreGpuObjectLoadedCondition);
	}

	void CoreObjectCore::synchronize()
	{
		if (!isInitialized())
		{
#if BS_DEBUG_MODE
			if (BS_THREAD_CURRENT_ID == CoreThread::instance().getCoreThreadId())
				BS_EXCEPT(InternalErrorException, "You cannot call this method on the core thread. It will cause a deadlock!");
#endif

			BS_LOCK_MUTEX_NAMED(mCoreGpuObjectLoadedMutex, lock);
			while (!isInitialized())
			{
				if (!isScheduledToBeInitialized())
					BS_EXCEPT(InternalErrorException, "Attempting to wait until initialization finishes but object is not scheduled to be initialized.");

				BS_THREAD_WAIT(mCoreGpuObjectLoadedCondition, mCoreGpuObjectLoadedMutex, lock);
			}
		}
	}

	void CoreObjectCore::_setThisPtr(std::shared_ptr<CoreObjectCore> ptrThis)
	{
		mThis = ptrThis;
	}
}