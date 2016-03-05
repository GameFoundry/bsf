//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsThreadPool.h"

#if BS_PLATFORM == BS_PLATFORM_WIN32
#include "windows.h"

#if BS_COMPILER == BS_COMPILER_MSVC
// disable: nonstandard extension used: 'X' uses SEH and 'Y' has destructor
// We don't care about this as any exception is meant to crash the program.
#pragma warning(disable: 4509)
#endif // BS_COMPILER == BS_COMPILER_MSVC

#endif // BS_PLATFORM == BS_PLATFORM_WIN32

namespace BansheeEngine
{
	HThread::HThread()
		:mThreadId(0), mPool(nullptr)
	{ }

	HThread::HThread(ThreadPool* pool, UINT32 threadId)
		:mThreadId(threadId), mPool(pool)
	{ }

	void HThread::blockUntilComplete()
	{
		PooledThread* parentThread = nullptr;

		{
			BS_LOCK_MUTEX(mPool->mMutex);

			for (auto& thread : mPool->mThreads)
			{
				if (thread->getId() == mThreadId)
				{
					parentThread = thread;
					break;
				}
			}
		}

		if (parentThread != nullptr)
		{
			BS_LOCK_MUTEX_NAMED(parentThread->mMutex, lock);

			if (parentThread->mId == mThreadId) // Check again in case it changed
			{
				while (!parentThread->mIdle)
					BS_THREAD_WAIT(parentThread->mWorkerEndedCond, parentThread->mMutex, lock);
			}
		}
	}

	PooledThread::PooledThread(const String& name)
		:mName(name), mId(0), mIdle(true), mThreadStarted(false),
		mThreadReady(false), mIdleTime(0)
	{ }

	PooledThread::~PooledThread()
	{ }

	void PooledThread::initialize()
	{
		BS_THREAD_CREATE(t, std::bind(&PooledThread::run, this));
		mThread = t;

		BS_LOCK_MUTEX_NAMED(mMutex, lock);

		while(!mThreadStarted)
			BS_THREAD_WAIT(mStartedCond, mMutex, lock);
	}

	void PooledThread::start(std::function<void()> workerMethod, UINT32 id)
	{
		{
			BS_LOCK_MUTEX(mMutex);

			mWorkerMethod = workerMethod;
			mIdle = false;
			mIdleTime = std::time(nullptr);
			mThreadReady = true;
			mId = id;
		}

		BS_THREAD_NOTIFY_ONE(mReadyCond);
	}

	void PooledThread::run()
	{
		onThreadStarted(mName);

		{
			BS_LOCK_MUTEX(mMutex);
			mThreadStarted = true;
		}

		BS_THREAD_NOTIFY_ONE(mStartedCond);

		while(true)
		{
			std::function<void()> worker = nullptr;

			{
				{
					BS_LOCK_MUTEX_NAMED(mMutex, lock);

					while (!mThreadReady)
						BS_THREAD_WAIT(mReadyCond, mMutex, lock);

					worker = mWorkerMethod;
				}

				if (worker == nullptr)
				{
					onThreadEnded(mName);
					return;
				}
			}

#if BS_PLATFORM == BS_PLATFORM_WIN32
			__try
			{
				worker();
			}
			__except (gCrashHandler().reportCrash(GetExceptionInformation()))
			{
				PlatformUtility::terminate(true);
			}
#else
			worker();
			LOGWRN("Starting a thread with no error handling.");
#endif

			{
				BS_LOCK_MUTEX(mMutex);

				mIdle = true;
				mIdleTime = std::time(nullptr);
				mThreadReady = false;
				mWorkerMethod = nullptr; // Make sure to clear as it could have bound shared pointers and similar

				BS_THREAD_NOTIFY_ONE(mWorkerEndedCond);
			}
		}
	}

	void PooledThread::destroy()
	{
		blockUntilComplete();

		{
			BS_LOCK_MUTEX(mMutex);
			mWorkerMethod = nullptr;
			mThreadReady = true;
		}

		BS_THREAD_NOTIFY_ONE(mReadyCond);
		BS_THREAD_JOIN((*mThread));
		BS_THREAD_DESTROY(mThread);
	}

	void PooledThread::blockUntilComplete()
	{
		BS_LOCK_MUTEX_NAMED(mMutex, lock);

		while (!mIdle)
			BS_THREAD_WAIT(mWorkerEndedCond, mMutex, lock);
	}

	bool PooledThread::isIdle()
	{
		BS_LOCK_MUTEX(mMutex);

		return mIdle;
	}

	time_t PooledThread::idleTime()
	{
		BS_LOCK_MUTEX(mMutex);

		return (time(nullptr) - mIdleTime);
	}

	void PooledThread::setName(const String& name)
	{
		mName = name;
	}

	UINT32 PooledThread::getId() const
	{ 
		BS_LOCK_MUTEX(mMutex);

		return mId; 
	}

	ThreadPool::ThreadPool(UINT32 threadCapacity, UINT32 maxCapacity, UINT32 idleTimeout)
		:mDefaultCapacity(threadCapacity), mMaxCapacity(maxCapacity), mIdleTimeout(idleTimeout), mAge(0)
	{

	}

	ThreadPool::~ThreadPool()
	{
		stopAll();
	}

	HThread ThreadPool::run(const String& name, std::function<void()> workerMethod)
	{
		PooledThread* thread = getThread(name);
		thread->start(workerMethod, mUniqueId++);

		return HThread(this, thread->getId());
	}

	void ThreadPool::stopAll()
	{
		BS_LOCK_MUTEX(mMutex);
		for(auto& thread : mThreads)
		{
			destroyThread(thread);
		}

		mThreads.clear();
	}

	void ThreadPool::clearUnused()
	{
		BS_LOCK_MUTEX(mMutex);
		mAge = 0;

		if(mThreads.size() <= mDefaultCapacity)
			return;

		Vector<PooledThread*> idleThreads;
		Vector<PooledThread*> expiredThreads;
		Vector<PooledThread*> activeThreads;

		idleThreads.reserve(mThreads.size());
		expiredThreads.reserve(mThreads.size());
		activeThreads.reserve(mThreads.size());

		for(auto& thread : mThreads)
		{
			if(thread->isIdle())
			{
				if(thread->idleTime() >= mIdleTimeout)
					expiredThreads.push_back(thread);
				else
					idleThreads.push_back(thread);
			}
			else
				activeThreads.push_back(thread);
		}

		idleThreads.insert(idleThreads.end(), expiredThreads.begin(), expiredThreads.end());
		UINT32 limit = std::min((UINT32)idleThreads.size(), mDefaultCapacity);

		UINT32 i = 0;
		mThreads.clear();

		for(auto& thread : idleThreads)
		{
			if(i < limit)
				mThreads.push_back(thread);
			else
				destroyThread(thread);
		}

		mThreads.insert(mThreads.end(), activeThreads.begin(), activeThreads.end());
	}

	void ThreadPool::destroyThread(PooledThread* thread)
	{
		thread->destroy();
		bs_delete(thread);
	}

	PooledThread* ThreadPool::getThread(const String& name)
	{
		UINT32 age = 0;
		{
			BS_LOCK_MUTEX(mMutex);
			age = ++mAge;
		}

		if(age == 32)
			clearUnused();

		PooledThread* newThread = nullptr;
		BS_LOCK_MUTEX(mMutex);

		for(auto& thread : mThreads)
		{
			if(thread->isIdle())
			{
				thread->setName(name);
				return thread;
			}
		}

		if(newThread == nullptr)
		{
			if(mThreads.size() >= mMaxCapacity)
				BS_EXCEPT(InvalidStateException, "Unable to create a new thread in the pool because maximum capacity has been reached.");

			newThread = createThread(name);
			mThreads.push_back(newThread);
		}

		return newThread;
	}

	UINT32 ThreadPool::getNumAvailable() const
	{
		UINT32 numAvailable = 0;

		BS_LOCK_MUTEX(mMutex);
		for(auto& thread : mThreads)
		{
			if(thread->isIdle())
				numAvailable++;
		}

		return numAvailable;
	}

	UINT32 ThreadPool::getNumActive() const
	{
		UINT32 numActive = 0;

		BS_LOCK_MUTEX(mMutex);
		for(auto& thread : mThreads)
		{
			if(!thread->isIdle())
				numActive++;
		}

		return numActive;
	}

	UINT32 ThreadPool::getNumAllocated() const
	{
		BS_LOCK_MUTEX(mMutex);

		return (UINT32)mThreads.size();
	}
}