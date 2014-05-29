#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Task priority. Tasks with higher priority will get executed sooner.
	 */
	enum class TaskPriority
	{
		VeryLow = 98,
		Low = 99,
		Normal = 100,
		High = 101,
		VeryHigh = 102
	};

	/**
	 * @brief	Represents a single task that may be queued in the TaskScheduler.
	 * 			
	 * @note	Thread safe.
	 */
	class BS_UTILITY_EXPORT Task
	{
		struct PrivatelyConstruct {};

	public:
		Task(const PrivatelyConstruct& dummy, const String& name, std::function<void()> taskWorker, 
			TaskPriority priority, TaskPtr dependency);

		/**
		 * @brief	Creates a new task. Task should be provided to TaskScheduler in order for it
		 * 			to start.
		 *
		 * @param	name		Name you can use to more easily identify the task.
		 * @param	taskWorker	Worker method that does all of the work in the task.
		 * @param	priority  	(optional) Higher priority means the tasks will be executed sooner.
		 * @param	dependency	(optional) Task dependency if one exists. If provided the task will
		 * 						not be executed until its dependency is complete.
		 */
		static TaskPtr create(const String& name, std::function<void()> taskWorker, TaskPriority priority = TaskPriority::Normal, 
			TaskPtr dependency = nullptr);

		/**
		 * @brief	Returns true if the task has completed.
		 */
		bool isComplete() const;

		/**
		 * @brief	Returns true if the task has been canceled.
		 */
		bool isCanceled() const;

		/**
		 * @brief	Blocks the current thread until the task has completed. 
		 * 			
		 * @note	While waiting adds a new worker thread, so that the blocking threads core can be utilized.
		 */
		void wait();

		/**
		 * @brief	Cancels the task and removes it from the TaskSchedulers queue.
		 */
		void cancel();

	private:
		friend class TaskScheduler;

		String mName;
		TaskPriority mPriority;
		UINT32 mTaskId;
		std::function<void()> mTaskWorker;
		TaskPtr mTaskDependency;
		std::atomic<UINT32> mState; /**< 0 - Inactive, 1 - In progress, 2 - Completed, 3 - Canceled */

		TaskScheduler* mParent;
	};

	/**
	 * @brief	Represents a task scheduler running on multiple threads. You may queue
	 * 			tasks on it from any thread and they will be executed in user specified order
	 * 			on any available thread.
	 * 			
	 * @note	Thread safe.
	 * 			
	 *			This type of task scheduler uses a global queue and is best used for coarse granularity of tasks.
	 *			(Number of tasks in the order of hundreds. Higher number of tasks might require different queuing and
	 *			locking mechanism, potentially at the cost of flexibility.)
	 *			
	 *			By default the task scheduler will create as many threads as there are physical CPU cores. You may add or remove
	 *			threads using addWorker/removeWorker methods.
	 */
	class BS_UTILITY_EXPORT TaskScheduler : public Module<TaskScheduler>
	{
	public:
		TaskScheduler();
		~TaskScheduler();

		/**
		 * @brief	Queues a new task.
		 */
		void addTask(const TaskPtr& task);

		/**
		 * @brief	Adds a new worker thread which will be used for executing queued tasks.
		 */
		void addWorker();

		/**
		 * @brief	Removes a worker thread (as soon as its current task is finished).
		 */
		void removeWorker();

	protected:
		friend class Task;

		/**
		 * @brief	Main task scheduler method that dispatches tasks to other threads.
		 */
		void runMain();

		/**
		 * @brief	Worker method that runs a single task.
		 */
		void runTask(const TaskPtr& task);

		/**
		 * @brief	Blocks the calling thread until the specified task has completed.
		 */
		void waitUntilComplete(const Task* task);

		/**
		 * @brief	Method used for sorting tasks.
		 */
		static bool taskCompare(const TaskPtr& lhs, const TaskPtr& rhs);

		Set<TaskPtr, std::function<bool(const TaskPtr&, const TaskPtr&)>> mTaskQueue;
		UINT32 mNumActiveTasks;
		UINT32 mMaxActiveTasks;
		UINT32 mNextTaskId;
		bool mShutdown;

		BS_MUTEX(mReadyMutex);
		BS_MUTEX(mCompleteMutex);
		BS_THREAD_SYNCHRONISER(mTaskReadyCond);
		BS_THREAD_SYNCHRONISER(mTaskCompleteCond);
	};
}