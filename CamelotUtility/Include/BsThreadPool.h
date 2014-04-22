#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class PooledThread;

	class CM_UTILITY_EXPORT ThreadPool : public Module<ThreadPool>
	{
	public:
		ThreadPool(UINT32 threadCapacity, UINT32 maxCapacity = 16, UINT32 idleTimeout = 60);
		~ThreadPool();

		void run(std::function<void()> workerMethod);

		void stopAll();

		void clearUnused();

		UINT32 getNumAvailable() const;
		UINT32 getNumActive() const;
		UINT32 getNumAllocated() const;

	private:
		Vector<PooledThread*>::type mThreads;
		
		PooledThread* createThread();
		void destroyThread(PooledThread* thread);
		PooledThread* getThread();

		UINT32 mDefaultCapacity;
		UINT32 mMaxCapacity;
		UINT32 mIdleTimeout;
		UINT32 mAge;
		
		CM_MUTEX(mMutex);
	};
}