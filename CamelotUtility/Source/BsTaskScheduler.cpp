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
		return cm_shared_ptr<Task>(PrivatelyConstruct(), name, taskWorker, priority, dependency);
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
		mMaxActiveTasks = CM_THREAD_HARDWARE_CONCURRENCY;

		ThreadPool::instance().run("TaskScheduler", std::bind(&TaskScheduler::runMain, this));
	}

	TaskScheduler::~TaskScheduler()
	{
		CM_LOCK_MUTEX(mReadyMutex);

		mShutdown = true;
		CM_THREAD_NOTIFY_ONE(mTaskReadyCond);
	}

	void TaskScheduler::addTask(const TaskPtr& task)
	{
		CM_LOCK_MUTEX(mReadyMutex);

		task->mParent = this;
		task->mTaskId = mNextTaskId++;

		mTaskQueue.insert(task);

		// Wake main scheduler thread
		CM_THREAD_NOTIFY_ONE(mTaskReadyCond);
	}

	void TaskScheduler::addWorker()
	{
		CM_LOCK_MUTEX(mReadyMutex);

		mMaxActiveTasks++;

		// A spot freed up, queue new tasks on main scheduler thread if they exist
		CM_THREAD_NOTIFY_ONE(mTaskReadyCond);
	}

	void TaskScheduler::removeWorker()
	{
		CM_LOCK_MUTEX(mReadyMutex);

		if(mMaxActiveTasks > 0)
			mMaxActiveTasks--;
	}

	void TaskScheduler::runMain()
	{
		while(true)
		{
			CM_LOCK_MUTEX_NAMED(mReadyMutex, lock);

			while((mTaskQueue.size() == 0 || mNumActiveTasks == mMaxActiveTasks) && !mShutdown)
				CM_THREAD_WAIT(mTaskReadyCond, mReadyMutex, lock);

			if(mShutdown)
				break;

			for(UINT32 i = 0; i < mTaskQueue.size(), mNumActiveTasks < mMaxActiveTasks; i++)
			{
				TaskPtr curTask = *mTaskQueue.begin();
				mTaskQueue.erase(mTaskQueue.begin());

				if(curTask->isCanceled())
					continue;

				if(curTask->mTaskDependency != nullptr && !curTask->mTaskDependency->isComplete())
					continue;

				curTask->mState.store(1);
				mNumActiveTasks++;

				ThreadPool::instance().run(curTask->mName, std::bind(&TaskScheduler::runTask, this, std::cref(curTask)));
			}
		}
	}

	void TaskScheduler::runTask(const TaskPtr& task)
	{
		task->mTaskWorker();

		{
			CM_LOCK_MUTEX(mCompleteMutex);
			task->mState.store(2);

			CM_THREAD_NOTIFY_ALL(mTaskCompleteCond);
		}

		// Possibly this task was someones dependency, so wake the main scheduler thread
		CM_THREAD_NOTIFY_ONE(mTaskReadyCond);
	}

	void TaskScheduler::waitUntilComplete(const Task* task)
	{
		if(task->isCanceled())
			return;

		{
			CM_LOCK_MUTEX_NAMED(mCompleteMutex, lock);
			
			while(!task->isComplete())
			{
				addWorker();
				CM_THREAD_WAIT(mTaskCompleteCond, mCompleteMutex, lock);
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
		if(lhs->mTaskId < rhs->mTaskId)
			return true;

		CM_EXCEPT(InternalErrorException, "Found two identical tasks.");
	}
}