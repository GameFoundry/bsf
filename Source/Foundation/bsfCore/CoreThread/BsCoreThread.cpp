//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "CoreThread/BsCoreThread.h"
#include "Threading/BsThreadPool.h"
#include "Threading/BsTaskScheduler.h"
#include "BsCoreApplication.h"

using namespace std::placeholders;

namespace bs
{
	CoreThread::QueueData CoreThread::mPerThreadQueue;
	BS_THREADLOCAL CoreThread::ThreadQueueContainer* CoreThread::QueueData::current = nullptr;

#if BS_CORE_THREAD_IS_MAIN
	bool CoreThread::sAppStarted = false;
	Mutex CoreThread::sAppStartedMutex;
	Signal CoreThread::sAppStartedCondition;
#endif

	void CoreThread::onStartUp()
	{
		for (UINT32 i = 0; i < NUM_SYNC_BUFFERS; i++)
		{
			mFrameAllocs[i] = bs_new<FrameAlloc>();
			mFrameAllocs[i]->setOwnerThread(BS_THREAD_CURRENT_ID); // Sim thread
		}

		mSimThreadId = BS_THREAD_CURRENT_ID;
		mCoreThreadId = mSimThreadId; // For now
		mCommandQueue = bs_new<CommandQueue<CommandQueueSync>>(BS_THREAD_CURRENT_ID);

		initCoreThread();
	}

	CoreThread::~CoreThread()
	{
		// TODO - What if something gets queued between the queued call to destroy_internal and this!?
		shutdownCoreThread();

		{
			Lock lock(mSubmitMutex);

			for(auto& queue : mAllQueues)
				bs_delete(queue);

			mAllQueues.clear();
		}

		if(mCommandQueue != nullptr)
		{
			bs_delete(mCommandQueue);
			mCommandQueue = nullptr;
		}

		for (UINT32 i = 0; i < NUM_SYNC_BUFFERS; i++)
		{
			mFrameAllocs[i]->setOwnerThread(BS_THREAD_CURRENT_ID); // Sim thread
			bs_delete(mFrameAllocs[i]);
		}
	}

	void CoreThread::initCoreThread()
	{
#if !BS_FORCE_SINGLETHREADED_RENDERING
#if !BS_CORE_THREAD_IS_MAIN
		mCoreThread = ThreadPool::instance().run("Core", std::bind(&CoreThread::runCoreThread, this));
#else
		{
			Lock lock(sAppStartedMutex);
			sAppStarted = true;
		}

		sAppStartedCondition.notify_one();
#endif
		
		// Need to wait to unsure thread ID is correctly set before continuing
		Lock lock(mThreadStartedMutex);

		while (!mCoreThreadStarted)
			mCoreThreadStartedCondition.wait(lock);
#endif
	}

#if BS_CORE_THREAD_IS_MAIN
	void CoreThread::_run()
	{
		// Wait for the application to reach a point where core thread can be safely started
		{
			Lock lock(sAppStartedMutex);

			while (!sAppStarted)
				sAppStartedCondition.wait(lock);
		}

		ThreadDefaultPolicy::onThreadStarted("Core");
		instance().runCoreThread();
		ThreadDefaultPolicy::onThreadEnded("Core");
	}
#endif

	void CoreThread::runCoreThread()
	{
#if !BS_FORCE_SINGLETHREADED_RENDERING
		TaskScheduler::instance().removeWorker(); // One less worker because we are reserving one core for this thread

		{
			Lock lock(mThreadStartedMutex);

			mCoreThreadStarted = true;
			mCoreThreadId = BS_THREAD_CURRENT_ID;
		}

		mCoreThreadStartedCondition.notify_one();

		while(true)
		{
			// Wait until we get some ready commands
			Queue<QueuedCommand>* commands = nullptr;
			{
				Lock lock(mCommandQueueMutex);

				while(mCommandQueue->isEmpty())
				{
					if(mCoreThreadShutdown)
					{
						TaskScheduler::instance().addWorker();
						return;
					}

					TaskScheduler::instance().addWorker(); // Do something else while we wait, otherwise this core will be unused
					mCommandReadyCondition.wait(lock);
					TaskScheduler::instance().removeWorker();
				}

				commands = mCommandQueue->flush();
			}

			// Play commands
			mCommandQueue->playbackWithNotify(commands, std::bind(&CoreThread::commandCompletedNotify, this, _1));
		}
#endif
	}

	void CoreThread::shutdownCoreThread()
	{
#if !BS_FORCE_SINGLETHREADED_RENDERING

		{
			Lock lock(mCommandQueueMutex);
			mCoreThreadShutdown = true;
		}

		// Wake all threads. They will quit after they see the shutdown flag
		mCommandReadyCondition.notify_all();

		mCoreThreadId = BS_THREAD_CURRENT_ID;

#if !BS_CORE_THREAD_IS_MAIN
		mCoreThread.blockUntilComplete();
#endif
#endif
	}

	SPtr<CommandQueue<CommandQueueSync>> CoreThread::getQueue()
	{
		if(mPerThreadQueue.current == nullptr)
		{
			SPtr<CommandQueue<CommandQueueSync>> newQueue = bs_shared_ptr_new<CommandQueue<CommandQueueSync>>(BS_THREAD_CURRENT_ID);
			mPerThreadQueue.current = bs_new<ThreadQueueContainer>();
			mPerThreadQueue.current->queue = newQueue;
			mPerThreadQueue.current->isMain = BS_THREAD_CURRENT_ID == mSimThreadId;

			Lock lock(mSubmitMutex);
			mAllQueues.push_back(mPerThreadQueue.current);
		}

		return mPerThreadQueue.current->queue;
	}

	void CoreThread::submitCommandQueue(CommandQueue<CommandQueueSync>& queue, bool blockUntilComplete)
	{
		Queue<QueuedCommand>* commands = queue.flush();

		CoreThreadQueueFlags flags = CTQF_InternalQueue;

		if(blockUntilComplete)
			flags |= CTQF_BlockUntilComplete;

		queueCommand(std::bind(&CommandQueueBase::playback, &queue, commands), flags);
	}

	void CoreThread::submitAll(bool blockUntilComplete)
	{
		UINT32 blockCommandId = (UINT32)-1;

		{
			// This lock is needed mainly because of blocking. Without it another submitting thread might flush a command
			// we want to wait on.
			Lock lock(mSubmitMutex);

			// Submit workers first
			ThreadQueueContainer* mainQueue = nullptr;
			for (auto& queue : mAllQueues)
			{
				if (!queue->isMain)
					submitCommandQueue(*queue->queue, false);
				else
					mainQueue = queue;
			}

			// Then main
			if (mainQueue != nullptr)
				submitCommandQueue(*mainQueue->queue, false);

			if(blockUntilComplete)
			{
				Lock lock2(mCommandQueueMutex);

				blockCommandId = mMaxCommandNotifyId++;
				mCommandQueue->queue([](){}, true, blockCommandId);
			}
		}

		if(blockUntilComplete)
		{
			mCommandReadyCondition.notify_all();
			blockUntilCommandCompleted(blockCommandId);
		}
	}

	void CoreThread::submit(bool blockUntilComplete)
	{
		Lock lock(mSubmitMutex);

		CommandQueue<CommandQueueSync>& queue = *getQueue();
		Queue<QueuedCommand>* commands = queue.flush();

		UINT32 commandId = -1;
		{
			Lock lock2(mCommandQueueMutex);

			if (blockUntilComplete)
			{
				commandId = mMaxCommandNotifyId++;

				mCommandQueue->queue([commands, &queue]() { queue.playback(commands); }, true, commandId);
			}
			else
				mCommandQueue->queue([commands, &queue]() { queue.playback(commands); });
		}

		mCommandReadyCondition.notify_all();

		if (blockUntilComplete)
			blockUntilCommandCompleted(commandId);
	}

	AsyncOp CoreThread::queueReturnCommand(std::function<void(AsyncOp&)> commandCallback, CoreThreadQueueFlags flags)
	{
#if !BS_FORCE_SINGLETHREADED_RENDERING
		assert(BS_THREAD_CURRENT_ID != getCoreThreadId() && "Cannot queue commands on the core thread for the core thread");
#endif

		if (!flags.isSet(CTQF_InternalQueue))
			return getQueue()->queueReturn(commandCallback);
		else
		{
			bool blockUntilComplete = flags.isSet(CTQF_BlockUntilComplete);

			AsyncOp op;
			UINT32 commandId = -1;
			{
				Lock lock(mCommandQueueMutex);

				if (blockUntilComplete)
				{
					commandId = mMaxCommandNotifyId++;
					op = mCommandQueue->queueReturn(commandCallback, true, commandId);
				}
				else
					op = mCommandQueue->queueReturn(commandCallback);
			}

			mCommandReadyCondition.notify_all();

			if (blockUntilComplete)
				blockUntilCommandCompleted(commandId);

			return op;
		}
	}

	void CoreThread::queueCommand(std::function<void()> commandCallback, CoreThreadQueueFlags flags)
	{
#if !BS_FORCE_SINGLETHREADED_RENDERING
		assert(BS_THREAD_CURRENT_ID != getCoreThreadId() && "Cannot queue commands on the core thread for the core thread");
#endif

		if (!flags.isSet(CTQF_InternalQueue))
			getQueue()->queue(commandCallback);
		else
		{
			bool blockUntilComplete = flags.isSet(CTQF_BlockUntilComplete);

			UINT32 commandId = -1;
			{
				Lock lock(mCommandQueueMutex);

				if (blockUntilComplete)
				{
					commandId = mMaxCommandNotifyId++;
					mCommandQueue->queue(commandCallback, true, commandId);
				}
				else
					mCommandQueue->queue(commandCallback);
			}

			mCommandReadyCondition.notify_all();

			if (blockUntilComplete)
				blockUntilCommandCompleted(commandId);
		}
	}

	void CoreThread::update()
	{
		for (UINT32 i = 0; i < NUM_SYNC_BUFFERS; i++)
			mFrameAllocs[i]->setOwnerThread(mCoreThreadId);

		mActiveFrameAlloc = (mActiveFrameAlloc + 1) % 2;
		mFrameAllocs[mActiveFrameAlloc]->setOwnerThread(BS_THREAD_CURRENT_ID); // Sim thread
		mFrameAllocs[mActiveFrameAlloc]->clear();
	}

	FrameAlloc* CoreThread::getFrameAlloc() const
	{
		return mFrameAllocs[mActiveFrameAlloc];
	}

	void CoreThread::blockUntilCommandCompleted(UINT32 commandId)
	{
#if !BS_FORCE_SINGLETHREADED_RENDERING

		while(true)
		{
			Lock lock(mCommandNotifyMutex);

			// Check if our command id is in the completed list
			auto iter = mCommandsCompleted.begin();
			for(; iter != mCommandsCompleted.end(); ++iter)
			{
				if(*iter == commandId)
				{
					mCommandsCompleted.erase(iter);
					return;
				}
			}

			mCommandCompleteCondition.wait(lock);
		}
#endif
	}

	void CoreThread::commandCompletedNotify(UINT32 commandId)
	{
		{
			Lock lock(mCommandNotifyMutex);
			mCommandsCompleted.push_back(commandId);
		}

		mCommandCompleteCondition.notify_all();
	}

	CoreThread& gCoreThread()
	{
		return CoreThread::instance();
	}

	void throwIfNotCoreThread()
	{
#if !BS_FORCE_SINGLETHREADED_RENDERING
		if(BS_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId())
			BS_EXCEPT(InternalErrorException, "This method can only be accessed from the core thread.");
#endif
	}

	void throwIfCoreThread()
	{
#if !BS_FORCE_SINGLETHREADED_RENDERING
		if(BS_THREAD_CURRENT_ID == CoreThread::instance().getCoreThreadId())
			BS_EXCEPT(InternalErrorException, "This method cannot be accessed from the core thread.");
#endif
	}
}
