#include "BsTaskScheduler.h"
#include "BsThreadPool.h"

namespace BansheeEngine
{
	Task::Task(const PrivatelyConstruct& dummy, const String& name, std::function<void()> taskWorker, 
		TaskPriority priority, TaskPtr dependency)
		:mName(name), mState(0), mPriority(priority), mTaskId(0), 
		mTaskDependency(dependency), mTaskWorker(taskWorker), mParent(nullptr)
	{

	}

	TaskPtr Task::create(const String& name, std::function<void()> taskWorker, TaskPriority priority, TaskPtr dependency)
	{
		return bs_shared_ptr<Task>(PrivatelyConstruct(), name, taskWorker, priority, dependency);
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
		:mMaxActiveTasks(0), mNumActiveTasks(0), mNextTaskId(0), mShutdown(false),
		mTaskQueue(&TaskScheduler::taskCompare)
	{
		mMaxActiveTasks = BS_THREAD_HARDWARE_CONCURRENCY;

		mTaskSchedulerThread = ThreadPool::instance().run("TaskScheduler", std::bind(&TaskScheduler::runMain, this));
	}

	TaskScheduler::~TaskScheduler()
	{
		// Wait until all tasks complete
		BS_LOCK_MUTEX_NAMED(mActiveTaskMutex, activeTaskLock);

		while (mActiveTasks.size() > 0)
		{
			TaskPtr task = mActiveTasks[0];
			activeTaskLock.unlock();

			task->wait();
			activeTaskLock.lock();
		}

		// Start shutdown of the main queue worker and wait until it exits
		{
			BS_LOCK_MUTEX(mReadyMutex);

			mShutdown = true;
		}

		BS_THREAD_NOTIFY_ONE(mTaskReadyCond);

		mTaskSchedulerThread.blockUntilComplete();
	}

	void TaskScheduler::addTask(const TaskPtr& task)
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

			while((mTaskQueue.size() == 0 || mNumActiveTasks == mMaxActiveTasks) && !mShutdown)
				BS_THREAD_WAIT(mTaskReadyCond, mReadyMutex, lock);

			if(mShutdown)
				break;

			for(UINT32 i = 0; (i < mTaskQueue.size()) && (mNumActiveTasks < mMaxActiveTasks); i++)
			{
				TaskPtr curTask = *mTaskQueue.begin();
				mTaskQueue.erase(mTaskQueue.begin());

				if(curTask->isCanceled())
					continue;

				if(curTask->mTaskDependency != nullptr && !curTask->mTaskDependency->isComplete())
					continue;

				BS_LOCK_MUTEX(mActiveTaskMutex);
				{
					curTask->mState.store(1);
					mActiveTasks.push_back(curTask);
					mNumActiveTasks++;
				}

				ThreadPool::instance().run(curTask->mName, std::bind(&TaskScheduler::runTask, this, curTask));
			}
		}
	}

	void TaskScheduler::runTask(TaskPtr task)
	{
		task->mTaskWorker();

		{
			BS_LOCK_MUTEX(mActiveTaskMutex);

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

	bool TaskScheduler::taskCompare(const TaskPtr& lhs, const TaskPtr& rhs)
	{
		// If one tasks priority is higher, that one goes first
		if(lhs->mPriority > rhs->mPriority)
			return true;

		// Otherwise we go by smaller id, as that task was queued earlier than the other
		return lhs->mTaskId < rhs->mTaskId;
	}
}