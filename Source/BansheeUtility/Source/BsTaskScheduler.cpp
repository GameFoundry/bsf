//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsTaskScheduler.h"
#include "BsThreadPool.h"

namespace BansheeEngine
{
	Task::Task(const PrivatelyConstruct& dummy, const String& name, std::function<void()> taskWorker, 
		TaskPriority priority, SPtr<Task> dependency)
		:mName(name), mPriority(priority), mTaskId(0), mTaskWorker(taskWorker), mTaskDependency(dependency),
		mParent(nullptr)
	{

	}

	SPtr<Task> Task::create(const String& name, std::function<void()> taskWorker, TaskPriority priority, SPtr<Task> dependency)
	{
		return bs_shared_ptr_new<Task>(PrivatelyConstruct(), name, taskWorker, priority, dependency);
	}

	bool Task::isComplete() const
	{
		return mState.load() == 2;
	}

	bool Task::isCanceled() const
	{
		return mState.load() == 3;
	}

	void Task::wait()
	{
		mParent->waitUntilComplete(this);
	}

	void Task::cancel()
	{
		mState.store(3);
	}

	TaskScheduler::TaskScheduler()
		:mTaskQueue(&TaskScheduler::taskCompare), mMaxActiveTasks(0), mNextTaskId(0), mShutdown(false)
	{
		mMaxActiveTasks = BS_THREAD_HARDWARE_CONCURRENCY;

		mTaskSchedulerThread = ThreadPool::instance().run("TaskScheduler", std::bind(&TaskScheduler::runMain, this));
	}

	TaskScheduler::~TaskScheduler()
	{
		// Wait until all tasks complete
		{
			BS_LOCK_MUTEX_NAMED(mReadyMutex, activeTaskLock);

			while (mActiveTasks.size() > 0)
			{
				SPtr<Task> task = mActiveTasks[0];
				activeTaskLock.unlock();

				task->wait();
				activeTaskLock.lock();
			}
		}

		// Start shutdown of the main queue worker and wait until it exits
		{
			BS_LOCK_MUTEX(mReadyMutex);

			mShutdown = true;
		}

		BS_THREAD_NOTIFY_ONE(mTaskReadyCond);

		mTaskSchedulerThread.blockUntilComplete();
	}

	void TaskScheduler::addTask(const SPtr<Task>& task)
	{
		BS_LOCK_MUTEX(mReadyMutex);

		task->mParent = this;
		task->mTaskId = mNextTaskId++;

		mTaskQueue.insert(task);

		// Wake main scheduler thread
		BS_THREAD_NOTIFY_ONE(mTaskReadyCond);
	}

	void TaskScheduler::addWorker()
	{
		BS_LOCK_MUTEX(mReadyMutex);

		mMaxActiveTasks++;

		// A spot freed up, queue new tasks on main scheduler thread if they exist
		BS_THREAD_NOTIFY_ONE(mTaskReadyCond);
	}

	void TaskScheduler::removeWorker()
	{
		BS_LOCK_MUTEX(mReadyMutex);

		if(mMaxActiveTasks > 0)
			mMaxActiveTasks--;
	}

	void TaskScheduler::runMain()
	{
		while(true)
		{
			BS_LOCK_MUTEX_NAMED(mReadyMutex, lock);

			while((mTaskQueue.size() == 0 || (UINT32)mActiveTasks.size() >= mMaxActiveTasks) && !mShutdown)
				BS_THREAD_WAIT(mTaskReadyCond, mReadyMutex, lock);

			if(mShutdown)
				break;

			for(UINT32 i = 0; (i < mTaskQueue.size()) && ((UINT32)mActiveTasks.size() < mMaxActiveTasks); i++)
			{
				SPtr<Task> curTask = *mTaskQueue.begin();
				mTaskQueue.erase(mTaskQueue.begin());

				if(curTask->isCanceled())
					continue;

				if(curTask->mTaskDependency != nullptr && !curTask->mTaskDependency->isComplete())
					continue;

				curTask->mState.store(1);
				mActiveTasks.push_back(curTask);

				ThreadPool::instance().run(curTask->mName, std::bind(&TaskScheduler::runTask, this, curTask));
			}
		}
	}

	void TaskScheduler::runTask(SPtr<Task> task)
	{
		task->mTaskWorker();

		{
			BS_LOCK_MUTEX(mReadyMutex);

			auto findIter = std::find(mActiveTasks.begin(), mActiveTasks.end(), task);
			if (findIter != mActiveTasks.end())
				mActiveTasks.erase(findIter);
		}

		{
			BS_LOCK_MUTEX(mCompleteMutex);
			task->mState.store(2);

			BS_THREAD_NOTIFY_ALL(mTaskCompleteCond);
		}

		// Possibly this task was someones dependency, so wake the main scheduler thread
		BS_THREAD_NOTIFY_ONE(mTaskReadyCond);
	}

	void TaskScheduler::waitUntilComplete(const Task* task)
	{
		if(task->isCanceled())
			return;

		{
			BS_LOCK_MUTEX_NAMED(mCompleteMutex, lock);
			
			while(!task->isComplete())
			{
				addWorker();
				BS_THREAD_WAIT(mTaskCompleteCond, mCompleteMutex, lock);
				removeWorker();
			}
		}
	}

	bool TaskScheduler::taskCompare(const SPtr<Task>& lhs, const SPtr<Task>& rhs)
	{
		// If one tasks priority is higher, that one goes first
		if(lhs->mPriority > rhs->mPriority)
			return true;

		// Otherwise we go by smaller id, as that task was queued earlier than the other
		return lhs->mTaskId < rhs->mTaskId;
	}
}