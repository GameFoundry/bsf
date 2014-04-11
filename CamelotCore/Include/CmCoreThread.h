#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmCommandQueue.h"
#include "CmCoreThreadAccessor.h"

namespace CamelotFramework
{
	/**
	 * @brief	Manager for the core thread. Takes care of starting, running, queuing commands
	 * 			and shutting down the core thread.
	 * 			
	 * @note	How threading works:
	 * 			 - This class contains a queue which is filled by commands from other threads via queueCommand and queueReturnCommand  
	 * 			 - Commands are executed on the core thread as soon as they are queued (if core thread is not busy with previous commands)  
	 * 			 - Core thread accessors are helpers for queuing commands. They serve two purposes:  
	 * 				- They contain helper methods for various common Core thread commands.
	 * 				- They perform better than queuing each command directly using queueCommand or queueReturnCommand    
	 * 			 - Accessors contain a command queue of their own, and queuing commands in them will not automatically start executing the commands  
	 * 			   like with queueCommand or queueReturnCommand. Instead you must manually call "submitAccessors" when you are ready to send their
	 * 			   commands to the core thread.
	 * 			 - Synced accessor is a special type of accessor which may be accessed from any thread. Its commands are always executed after all other  
	 * 			   non-synced accessors. It is primarily useful when multiple threads are managing the same resource and you must ensure proper order of operations.
	 * 			   You should use normal accessors whenever possible as synced accessors involve potentially slow synchronization operations.
	 */
	class CoreThread : public Module<CoreThread>
	{
		struct AccessorContainer
		{
			CoreAccessorPtr accessor;
		};

public:
	CM_EXPORT CoreThread();
	CM_EXPORT ~CoreThread();

	/**
		* @brief	Returns the id of the core thread. If a separate core thread
		* 			is not used, then it returns the id of the thread RenderSystem
		* 			was initialized on.
		*/
	CM_EXPORT CM_THREAD_ID_TYPE getCoreThreadId() { return mCoreThreadId; }

	/**
		* @brief	Creates or retrieves an accessor that you can use for executing commands on the core thread from 
		* 			a non-core thread. The accessor will be bound to the thread you call this method on.
		* 			
		* @note		Accessors contain their own command queue and their commands will only start to get executed once that queue is submitted
		* 			to the core thread via "submitAccessors" method.
		*/
	CM_EXPORT CoreAccessorPtr getAccessor();

	/**
	* @brief	Retrieves an accessor that you can use for executing commands on the core thread from
	* 			a non-core thread. There is only one synchronized accessor and you may access it from any thread you wish.
	* 			Note however that it is much more efficient to create a separate non-synchronized accessor using
	* 			"createCoreAccessor" for each thread you will be using it on.
	* 			
	* @note		Accessors contain their own command queue and their commands will only start to get executed once that queue is submitted
	* 			to the core thread via "submitAccessors" method.
	* 			
	*			Synced accessor commands are sent after all non-synced accessor commands are sent.
	*/
	CM_EXPORT SyncedCoreAccessor& getSyncedAccessor();

	/**
	 * @brief	Queues all the accessor commands and starts executing them on the core thread.
	 */
	CM_EXPORT void submitAccessors(bool blockUntilComplete = false);

	/**
		* @brief	Queues a new command that will be added to the global command queue. You are allowed to call this from any thread,
		* 			however be aware that it involves possibly slow synchronization primitives, so limit your usage.
		* 			
		* @param	blockUntilComplete If true the thread will be blocked until the command executes. Be aware that there may be many commands queued before it
		* 							   and they all need to be executed in order before the current command is reached, which might take a long time.
		* 	
		* @see		CommandQueue::queueReturn
		*/
	CM_EXPORT AsyncOp queueReturnCommand(std::function<void(AsyncOp&)> commandCallback, bool blockUntilComplete = false);

	/**
	* @brief	Queues a new command that will be added to the global command queue.You are allowed to call this from any thread,
		* 			however be aware that it involves possibly slow synchronization primitives, so limit your usage.
		* 	
		* @param	blockUntilComplete If true the thread will be blocked until the command executes. Be aware that there may be many commands queued before it
		* 							   and they all need to be executed in order before the current command is reached, which might take a long time.
		* @see		CommandQueue::queue
		*/
	CM_EXPORT void queueCommand(std::function<void()> commandCallback, bool blockUntilComplete = false);

	/**
	 * @brief	Called once every frame.
	 * 			
	 * @note	Must be called before sim thread schedules any CoreThread operations that frame. 
	 */
	CM_EXPORT void update();

	/**
	 * @brief	Returns a frame allocator that should be used for allocating temporary data being passed to the
	 * 			core thread. As the name implies the data only lasts one frame, so you need to be careful not
	 * 			to use it for longer than that.
	 * 			
	 * @note	Sim thread only.
	 */
	CM_EXPORT FrameAlloc* getFrameAlloc() const;
private:
	class CoreThreadWorkerFunc CM_THREAD_WORKER_INHERIT
	{
	public:
		CoreThreadWorkerFunc(CoreThread* owner);

		void operator()();

	private:
		CoreThread* mOwner;
	};

	// Double buffered frame allocators - Means sim thread cannot be more than 1 frame ahead of core thread
	// (If that changes you should be able to easily add more)
	FrameAlloc* mFrameAllocs[2]; 
	UINT32 mActiveFrameAlloc;

	static CM_THREADLOCAL AccessorContainer* mAccessor;
	Vector<AccessorContainer*>::type mAccessors;

	CoreThreadWorkerFunc* mCoreThreadFunc;
	volatile bool mCoreThreadStarted;
	volatile bool mCoreThreadShutdown;

	CM_THREAD_ID_TYPE mCoreThreadId;
	CM_THREAD_SYNCHRONISER(mCoreThreadStartCondition)
	CM_MUTEX(mCoreThreadStartMutex)
	CM_MUTEX(mCommandQueueMutex)
	CM_MUTEX(mAccessorMutex)
	CM_THREAD_SYNCHRONISER(mCommandReadyCondition)
	CM_MUTEX(mCommandNotifyMutex)
	CM_THREAD_SYNCHRONISER(mCommandCompleteCondition)

#if CM_THREAD_SUPPORT
	CM_THREAD_TYPE* mCoreThread;
#endif

	CommandQueue<CommandQueueSync>* mCommandQueue;

	UINT32 mMaxCommandNotifyId; // ID that will be assigned to the next command with a notifier callback
	Vector<UINT32>::type mCommandsCompleted; // Completed commands that have notifier callbacks set up

	SyncedCoreAccessor* mSyncedCoreAccessor;

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

	CM_EXPORT CoreThreadAccessor<CommandQueueNoSync>& gCoreAccessor();

	CM_EXPORT CoreThreadAccessor<CommandQueueSync>& gSyncedCoreAccessor();

	/**
		* @brief	Throws an exception if current thread isn't the core thread;
		*/
	CM_EXPORT void throwIfNotCoreThread();

	/**
		* @brief	Throws an exception if current thread is the core thread;
		*/
	CM_EXPORT void throwIfCoreThread();

#if CM_DEBUG_MODE
#define THROW_IF_NOT_CORE_THREAD throwIfNotCoreThread();
#define THROW_IF_CORE_THREAD throwIfCoreThread();
#else
#define THROW_IF_NOT_CORE_THREAD 
#define THROW_IF_CORE_THREAD
#endif
}