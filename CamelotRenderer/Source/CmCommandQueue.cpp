#include "CmCommandQueue.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	CommandQueue::CommandQueue(CM_THREAD_ID_TYPE threadId, bool allowAllThreads)
		:mMyThreadId(threadId), mAllowAllThreads(allowAllThreads)
	{
		mCommands = new vector<Command>::type();
	}

	CommandQueue::~CommandQueue()
	{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
		if(!mAllowAllThreads && CM_THREAD_CURRENT_ID != mMyThreadId)
		{
			CM_EXCEPT(InternalErrorException, "Command queue accessed outside of its creation thread.");
		}
#endif
#endif

		if(mCommands != nullptr)
			delete mCommands;
	}

	AsyncOp CommandQueue::queueReturn(boost::function<void(AsyncOp&)> commandCallback, bool _notifyWhenComplete, UINT32 _callbackId)
	{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
		if(!mAllowAllThreads && CM_THREAD_CURRENT_ID != mMyThreadId)
		{
			CM_EXCEPT(InternalErrorException, "Command queue accessed outside of its creation thread.");
		}
#endif
#endif

		Command newCommand(commandCallback, _notifyWhenComplete, _callbackId);
		mCommands->push_back(newCommand);

		return newCommand.asyncOp;
	}

	void CommandQueue::queue(boost::function<void()> commandCallback, bool _notifyWhenComplete, UINT32 _callbackId)
	{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
		if(!mAllowAllThreads && CM_THREAD_CURRENT_ID != mMyThreadId)
		{
			CM_EXCEPT(InternalErrorException, "Command queue accessed outside of its creation thread.");
		}
#endif
#endif

		Command newCommand(commandCallback, _notifyWhenComplete, _callbackId);
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

	void CommandQueue::playback(vector<CommandQueue::Command>::type* commands, boost::function<void(UINT32)> notifyCallback)
	{
#if CM_DEBUG_MODE
		RenderSystem* rs = RenderSystem::instancePtr();

		if(rs->getRenderThreadId() != CM_THREAD_CURRENT_ID)
			CM_EXCEPT(InternalErrorException, "This method should only be called from the render thread.");
#endif

		if(commands == nullptr)
			return;

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

			if(command.notifyWhenComplete && !notifyCallback.empty())
			{
				notifyCallback(command.callbackId);
			}
		}

		delete commands;
	}

	void CommandQueue::playback(vector<Command>::type* commands)
	{
		playback(commands, boost::function<void(UINT32)>());
	}

	bool CommandQueue::isEmpty()
	{
		CM_LOCK_MUTEX(mCommandBufferMutex);

		if(mCommands != nullptr && mCommands->size() > 0)
			return false;

		return true;
	}
}