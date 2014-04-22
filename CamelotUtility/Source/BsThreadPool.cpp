#include "BsThreadPool.h"

namespace BansheeEngine
{
	class PooledThread
	{
	public:
		PooledThread()
			:mIdle(true), mThreadStarted(false),
			 mThreadReady(false), mIdleTime(0)
		{
			CM_THREAD_CREATE(t, std::bind(&PooledThread::run, this));

			CM_LOCK_MUTEX_NAMED(mMutex, lock);

			while(!mThreadStarted)
				CM_THREAD_WAIT(mStartedCond, mMutex, lock);
		}

		~PooledThread()
		{

		}

		void start(std::function<void()> workerMethod)
		{
			{
				CM_LOCK_MUTEX(mMutex);
				mWorkerMethod = workerMethod;
				mThreadReady = true;
			}

			CM_THREAD_NOTIFY_ONE(mReadyCond);
		}

		void run()
		{
			{
				CM_LOCK_MUTEX(mMutex);
				mThreadStarted = true;
			}

			CM_THREAD_NOTIFY_ONE(mStartedCond);

			while(true)
			{
				std::function<void()> worker = nullptr;

				{
					CM_LOCK_MUTEX_NAMED(mMutex, lock);

					while(!mThreadReady)
						CM_THREAD_WAIT(mReadyCond, mMutex, lock);

					if(mWorkerMethod == nullptr)
						return;

					worker = mWorkerMethod;
					mIdle = false;
				}

				worker();

				{
					CM_LOCK_MUTEX(mMutex);

					mIdle = true;
					mIdleTime = std::time(nullptr);
					mThreadReady = false;
				}
			}
		}

		void destroy()
		{
			{
				CM_LOCK_MUTEX(mMutex);
				mWorkerMethod = nullptr;
				mThreadReady = true;
			}

			CM_THREAD_NOTIFY_ONE(mReadyCond);
			CM_THREAD_JOIN((*mThread));
			CM_THREAD_DESTROY(mThread);
		}

		bool isIdle()
		{
			CM_LOCK_MUTEX(mMutex);

			return mIdle;
		}

		time_t idleTime()
		{
			CM_LOCK_MUTEX(mMutex);

			return (time(nullptr) - mIdleTime);
		}

		std::function<void()> mWorkerMethod;

		bool mIdle;
		bool mThreadStarted;
		bool mThreadReady;

		time_t mIdleTime;

		CM_THREAD_TYPE* mThread;
		CM_MUTEX(mMutex);
		CM_THREAD_SYNCHRONISER(mStartedCond);
		CM_THREAD_SYNCHRONISER(mReadyCond);
	};

	ThreadPool::ThreadPool(UINT32 threadCapacity, UINT32 maxCapacity, UINT32 idleTimeout)
		:mDefaultCapacity(threadCapacity), mMaxCapacity(maxCapacity), mIdleTimeout(idleTimeout), mAge(0)
	{
		for(UINT32 i = 0; i < mDefaultCapacity; i++)
		{
			mThreads.push_back(createThread());
		}
	}

	ThreadPool::~ThreadPool()
	{
		stopAll();
	}

	void ThreadPool::run(std::function<void()> workerMethod)
	{
		getThread()->start(workerMethod);
	}

	void ThreadPool::stopAll()
	{
		CM_LOCK_MUTEX(mMutex);
		for(auto& thread : mThreads)
		{
			destroyThread(thread);
		}

		mThreads.clear();
	}

	void ThreadPool::clearUnused()
	{
		CM_LOCK_MUTEX(mMutex);
		mAge = 0;

		if(mThreads.size() <= mDefaultCapacity)
			return;

		Vector<PooledThread*>::type idleThreads;
		Vector<PooledThread*>::type expiredThreads;
		Vector<PooledThread*>::type activeThreads;

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

	PooledThread* ThreadPool::createThread()
	{
		return cm_new<PooledThread>();
	}

	void ThreadPool::destroyThread(PooledThread* thread)
	{
		thread->destroy();
		cm_delete(thread);
	}

	PooledThread* ThreadPool::getThread()
	{
		UINT32 age = 0;
		{
			CM_LOCK_MUTEX(mMutex);
			age = ++mAge;
		}

		if(age == 32)
			clearUnused();

		PooledThread* newThread = nullptr;
		CM_LOCK_MUTEX(mMutex);

		for(auto& thread : mThreads)
		{
			if(thread->isIdle())
				return thread;
		}

		if(newThread == nullptr)
		{
			if(mThreads.size() >= mMaxCapacity)
				CM_EXCEPT(InvalidStateException, "Unable to create a new thread in the pool because maximum capacity has been reached.");

			newThread = createThread();
			mThreads.push_back(newThread);
		}

		return newThread;
	}

	UINT32 ThreadPool::getNumAvailable() const
	{
		UINT32 numAvailable = 0;

		CM_LOCK_MUTEX(mMutex);
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

		CM_LOCK_MUTEX(mMutex);
		for(auto& thread : mThreads)
		{
			if(!thread->isIdle())
				numActive++;
		}

		return numActive;
	}

	UINT32 ThreadPool::getNumAllocated() const
	{
		CM_LOCK_MUTEX(mMutex);

		return (UINT32)mThreads.size();
	}
}