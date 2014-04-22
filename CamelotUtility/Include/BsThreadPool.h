#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class CM_UTILITY_EXPORT PooledThread
	{
	public:
		PooledThread(const String& name);
		virtual ~PooledThread();

		void start(std::function<void()> workerMethod);
		void run();
		void destroy();

		bool isIdle();
		time_t idleTime();

		void setName(const String& name);

		virtual void onThreadStarted(const String& name) = 0;
		virtual void onThreadEnded(const String& name) = 0;

		std::function<void()> mWorkerMethod;

		String mName;
		bool mIdle;
		bool mThreadStarted;
		bool mThreadReady;

		time_t mIdleTime;

		CM_THREAD_TYPE* mThread;
		CM_MUTEX(mMutex);
		CM_THREAD_SYNCHRONISER(mStartedCond);
		CM_THREAD_SYNCHRONISER(mReadyCond);
	};

	template<class ThreadPolicy>
	class TPooledThread : public PooledThread
	{
	public:
		TPooledThread(const String& name)
			:PooledThread(name)
		{ }

		void onThreadStarted(const String& name)
		{
			ThreadPolicy::onThreadStarted(name);
		}

		void onThreadEnded(const String& name)
		{
			ThreadPolicy::onThreadEnded(name);
		}
	};

	class CM_UTILITY_EXPORT ThreadPoolBase : public Module<ThreadPoolBase>
	{
	public:
		ThreadPoolBase(UINT32 threadCapacity, UINT32 maxCapacity = 16, UINT32 idleTimeout = 60);
		virtual ~ThreadPoolBase();

		void run(const String& name, std::function<void()> workerMethod);

		void stopAll();

		void clearUnused();

		UINT32 getNumAvailable() const;
		UINT32 getNumActive() const;
		UINT32 getNumAllocated() const;

	protected:
		Vector<PooledThread*>::type mThreads;
		
		virtual PooledThread* createThread(const String& name) = 0;
		void destroyThread(PooledThread* thread);
		PooledThread* getThread(const String& name);

		UINT32 mDefaultCapacity;
		UINT32 mMaxCapacity;
		UINT32 mIdleTimeout;
		UINT32 mAge;
		
		CM_MUTEX(mMutex);
	};

	class ThreadNoPolicy
	{
	public:
		static void onThreadStarted(const String& name) { }
		static void onThreadEnded(const String& name) { }
	};

	template<class ThreadPolicy = ThreadNoPolicy>
	class ThreadPool : public ThreadPoolBase
	{
	public:
		ThreadPool(UINT32 threadCapacity, UINT32 maxCapacity = 16, UINT32 idleTimeout = 60)
			:ThreadPoolBase(threadCapacity, maxCapacity, idleTimeout)
		{

		}

	protected:
		PooledThread* createThread(const String& name)
		{
			return cm_new<TPooledThread<ThreadPolicy>>(name);
		}
	};
}