#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmCommandQueue.h"
#include "CmDeferredRenderContext.h"

namespace CamelotFramework
{
	/**
	 * @brief	Manager for the core thread. Takes care of starting, running, queuing commands
	 * 			and shutting down the core thread.
	 */
	class CM_EXPORT CoreThread : public Module<CoreThread>
	{
public:
	CoreThread();
	~CoreThread();

	/**
		* @brief	Returns the id of the core thread. If a separate core thread
		* 			is not used, then it returns the id of the thread RenderSystem
		* 			was initialized on.
		*/
	CM_THREAD_ID_TYPE getCoreThreadId() { return mCoreThreadId; }

	/**
		* @brief	Creates an accessor that you can use for executing commands on the core thread from 
		* 			a non-core thread. You can have as many of these as you wish, the only limitation
		* 			is that you do not use a single instance on more than one thread. Each thread
		* 			requires its own accessor. The accessor will be bound to the thread you call this method on.
		*/
	RenderContextPtr createAccessor();

	/**
	* @brief	Retrieves an accessor that you can use for executing commands on the core thread from
	* 			a non-core thread. There is only one synchronized accessor and you may access it from any thread you wish.
	* 			Note however that it is much more efficient to create a separate non-synchronized accessor using
	* 			"createCoreAccessor" for each thread you will be using it on.
		*/
	SyncedRenderContext& getSyncedAccessor();

	/**
		* @brief	Queues a new command that will be added to the global command queue. You are allowed to call this from any thread,
		* 			however be aware that it involves possibly slow synchronization primitives, so limit your usage.
		* 			
		* @param	blockUntilComplete If true the thread will be blocked until the command executes. Be aware that there be many commands queued before it
		* 							   and they all need to be executed in order before the current command is reached, which might take a long time.
		* 	
		* @see		CommandQueue::queueReturn
		*/
	AsyncOp queueReturnCommand(boost::function<void(AsyncOp&)> commandCallback, bool blockUntilComplete = false);

	/**
	* @brief	Queues a new command that will be added to the global command queue.You are allowed to call this from any thread,
		* 			however be aware that it involves possibly slow synchronization primitives, so limit your usage.
		* 	
		* @param	blockUntilComplete If true the thread will be blocked until the command executes. Be aware that there be many commands queued before it
		* 							   and they all need to be executed in order before the current command is reached, which might take a long time.
		* @see		CommandQueue::queue
		*/
	void queueCommand(boost::function<void()> commandCallback, bool blockUntilComplete = false);

private:
	class CoreThreadWorkerFunc CM_THREAD_WORKER_INHERIT
	{
	public:
		CoreThreadWorkerFunc(CoreThread* owner);

		void operator()();

	private:
		CoreThread* mOwner;
	};

	CoreThreadWorkerFunc* mCoreThreadFunc;
	volatile bool mCoreThreadStarted;
	volatile bool mCoreThreadShutdown;

	CM_THREAD_ID_TYPE mCoreThreadId;
	CM_THREAD_SYNCHRONISER(mCoreThreadStartCondition)
	CM_MUTEX(mCoreThreadStartMutex)
	CM_MUTEX(mCommandQueueMutex)
	CM_THREAD_SYNCHRONISER(mCommandReadyCondition)
	CM_MUTEX(mCommandNotifyMutex)
	CM_THREAD_SYNCHRONISER(mCommandCompleteCondition)

#if CM_THREAD_SUPPORT
	CM_THREAD_TYPE* mCoreThread;
#endif

	CommandQueue<CommandQueueSync>* mCommandQueue;

	UINT32 mMaxCommandNotifyId; // ID that will be assigned to the next command with a notifier callback
	Vector<UINT32>::type mCommandsCompleted; // Completed commands that have notifier callbacks set up

	SyncedRenderContext* mSyncedCoreAccessor;

	/**
		* @brief	Initializes a separate core thread. Should only be called once.
		*/
	void initCoreThread();

	/**
		* @brief	Main function of the core thread. Called once thread is started.
		*/
	void runCoreThread();

	/**
		* @brief	Shutdowns the core thread. It will complete all ready commands
		* 			before shutdown.
		*/
	void shutdownCoreThread();

	/**
		* @brief	Blocks the calling thread until the command with the specified ID completes.
		* 			Make sure that the specified ID actually exists, otherwise this will block forever.
		*/
	void blockUntilCommandCompleted(UINT32 commandId);

	/**
		* @brief	Callback called by the command list when a specific command finishes executing.
		* 			This is only called on commands that have a special notify on complete flag set.
		*
		* @param	commandId	Identifier for the command.
		*/
	void commandCompletedNotify(UINT32 commandId);
	};

	CM_EXPORT CoreThread& gCoreThread();

	/**
		* @brief	Throws an exception if current thread isn't the core thread;
		*/
	static void throwIfNotCoreThread();

#if CM_DEBUG_MODE
#define THROW_IF_NOT_CORE_THREAD throwIfNotCoreThread();
#else
#define THROW_IF_NOT_CORE_THREAD 
#endif
}