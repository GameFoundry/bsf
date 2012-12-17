#include "CmCommandQueue.h"
#include "CmException.h"
#include "CmRenderSystemManager.h"
#include "CmRenderSystem.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	CommandQueue::CommandQueue(CM_THREAD_ID_TYPE threadId)
		:mMyThreadId(threadId), mIsExecuting(false)
	{
		mCommands = new vector<Command>::type();
	}

	CommandQueue::~CommandQueue()
	{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
		if(CM_THREAD_CURRENT_ID != mMyThreadId)
		{
			CM_EXCEPT(InternalErrorException, "Command queue accessed outside of its creation thread.");
		}
#endif
#endif

		if(mCommands != nullptr)
			delete mCommands;
	}

	AsyncOp CommandQueue::queueReturn(boost::function<void(AsyncOp&)> commandCallback, UINT32 _callbackId)
	{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
		if(CM_THREAD_CURRENT_ID != mMyThreadId)
		{
			CM_EXCEPT(InternalErrorException, "Command queue accessed outside of its creation thread.");
		}
#endif
#endif

		Command newCommand(commandCallback, _callbackId);
		mCommands->push_back(newCommand);

		return newCommand.asyncOp;
	}

	void CommandQueue::queue(boost::function<void()> commandCallback, UINT32 _callbackId)
	{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
		if(CM_THREAD_CURRENT_ID != mMyThreadId)
		{
			CM_EXCEPT(InternalErrorException, "Command queue accessed outside of its creation thread.");
		}
#endif
#endif

		Command newCommand(commandCallback, _callbackId);
		mCommands->push_back(newCommand);
	}

	vector<CommandQueue::Command>::type* CommandQueue::flush()
	{
		vector<Command>::type* oldCommands = nullptr;
		{
			CM_LOCK_MUTEX(mCommandBufferMutex);

			oldCommands = mCommands;
			mCommands = new vector<Command>::type();
		}

		return oldCommands;
	}

	void CommandQueue::playback(vector<CommandQueue::Command>::type* commands)
	{
#if CM_DEBUG_MODE
		RenderSystem* rs = RenderSystemManager::getActive();

		if(rs->getRenderThreadId() != CM_THREAD_CURRENT_ID)
			CM_EXCEPT(InternalErrorException, "This method should only be called from the render thread.");
#endif

		{
			CM_LOCK_MUTEX(mCommandBufferMutex)

			mIsExecuting = true;
		}

		if(commands == nullptr)
		{
			{
				CM_LOCK_MUTEX(mCommandBufferMutex);
				mIsExecuting = false;
			}

			CM_THREAD_NOTIFY_ALL(mContextPlaybackDoneCondition);
			return;
		}

		for(auto iter = commands->begin(); iter != commands->end(); ++iter)
		{
			Command& command = (*iter);

			if(command.returnsValue)
			{
				command.callbackWithReturnValue(command.asyncOp);

				if(!command.asyncOp.hasCompleted())
				{
					LOGDBG("Async operation return value wasn't resolved properly. Resolving automatically to nullptr. " \
						"Make sure to complete the operation before returning from the command callback method.");
					command.asyncOp.completeOperation(nullptr);
				}
			}
			else
			{
				command.callback();
			}
		}

		delete commands;

		{
			CM_LOCK_MUTEX(mCommandBufferMutex);
			mIsExecuting = false;
		}

		CM_THREAD_NOTIFY_ALL(mContextPlaybackDoneCondition)
	}

	void CommandQueue::blockUntilExecuted()
	{
#if CM_DEBUG_MODE
		RenderSystem* rs = RenderSystemManager::getActive();

		if(rs->getRenderThreadId() == CM_THREAD_CURRENT_ID)
			CM_EXCEPT(InternalErrorException, "This method should never be called from the render thread as it will cause a deadlock.");
#endif

		{
			CM_LOCK_MUTEX_NAMED(mCommandBufferMutex, lock);
			while (mIsExecuting)
			{
				CM_THREAD_WAIT(mContextPlaybackDoneCondition, mCommandBufferMutex, lock)
			}
		}
	}
}