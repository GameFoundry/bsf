#include "CmCommandQueue.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmDebug.h"
#include "CmUtil.h"

namespace CamelotEngine
{
#if CM_DEBUG_MODE
	CommandQueue::CommandQueue(CM_THREAD_ID_TYPE threadId, bool allowAllThreads)
		:mMyThreadId(threadId), mAllowAllThreads(allowAllThreads), mMaxDebugIdx(0)
	{
		mCommands = CM_NEW(std::queue<Command>, PoolAlloc) std::queue<Command>();

		{
			CM_LOCK_MUTEX(CommandQueueBreakpointMutex);

			mCommandQueueIdx = MaxCommandQueueIdx++;
		}
	}
#else
	CommandQueue::CommandQueue(CM_THREAD_ID_TYPE threadId, bool allowAllThreads)
		:mMyThreadId(threadId), mAllowAllThreads(allowAllThreads)
	{
		mCommands = CM_NEW(std::queue<Command>, PoolAlloc) std::queue<Command>();
	}
#endif

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
			CM_DELETE(mCommands, std::queue<Command>, PoolAlloc);
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

#if CM_DEBUG_MODE
		breakIfNeeded(mCommandQueueIdx, mMaxDebugIdx);

		Command newCommand(commandCallback, mMaxDebugIdx++, _notifyWhenComplete, _callbackId);
#else
		Command newCommand(commandCallback, _notifyWhenComplete, _callbackId);
#endif

		mCommands->push(newCommand);

#if CM_FORCE_SINGLETHREADED_RENDERING
		std::queue<CommandQueue::Command>* commands = flush();
		playback(commands);
#endif

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

#if CM_DEBUG_MODE
		breakIfNeeded(mCommandQueueIdx, mMaxDebugIdx);

		Command newCommand(commandCallback, mMaxDebugIdx++, _notifyWhenComplete, _callbackId);
#else
		Command newCommand(commandCallback, _notifyWhenComplete, _callbackId);
#endif

		mCommands->push(newCommand);

#if CM_FORCE_SINGLETHREADED_RENDERING
		std::queue<CommandQueue::Command>* commands = flush();
		playback(commands);
#endif
	}

	std::queue<CommandQueue::Command>* CommandQueue::flush()
	{
		std::queue<Command>* oldCommands = nullptr;
		{
			CM_LOCK_MUTEX(mCommandBufferMutex);

			oldCommands = mCommands;
			mCommands = CM_NEW(std::queue<Command>, PoolAlloc) std::queue<Command>();
		}

		return oldCommands;
	}

	void CommandQueue::playback(std::queue<CommandQueue::Command>* commands, boost::function<void(UINT32)> notifyCallback)
	{
#if CM_DEBUG_MODE
		RenderSystem* rs = RenderSystem::instancePtr();

		if(rs->getRenderThreadId() != CM_THREAD_CURRENT_ID)
			CM_EXCEPT(InternalErrorException, "This method should only be called from the render thread.");
#endif

		if(commands == nullptr)
			return;

		while(!commands->empty())
		{
			Command& command = commands->front();

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

			commands->pop();
		}

		CM_DELETE(commands, std::queue<Command>, PoolAlloc);
	}

	void CommandQueue::playback(std::queue<Command>* commands)
	{
		playback(commands, boost::function<void(UINT32)>());
	}

	void CommandQueue::cancelAll()
	{
		std::queue<CommandQueue::Command>* commands = flush();
		CM_DELETE(commands, std::queue<Command>, PoolAlloc);
	}

	bool CommandQueue::isEmpty()
	{
		CM_LOCK_MUTEX(mCommandBufferMutex);

		if(mCommands != nullptr && mCommands->size() > 0)
			return false;

		return true;
	}

#if CM_DEBUG_MODE
	CM_STATIC_MUTEX_CLASS_INSTANCE(CommandQueueBreakpointMutex, CommandQueue);
	UINT32 CommandQueue::MaxCommandQueueIdx = 0;
	std::unordered_set<CommandQueue::QueueBreakpoint, CommandQueue::QueueBreakpoint::HashFunction, 
		CommandQueue::QueueBreakpoint::EqualFunction> CommandQueue::SetBreakpoints;

	inline size_t CommandQueue::QueueBreakpoint::HashFunction::operator()(const QueueBreakpoint& v) const
	{
		size_t seed = 0;
		hash_combine(seed, v.queueIdx);
		hash_combine(seed, v.commandIdx);
		return seed;
	}

	inline bool CommandQueue::QueueBreakpoint::EqualFunction::operator()(const QueueBreakpoint &a, const QueueBreakpoint &b) const
	{
		return a.queueIdx == b.queueIdx && a.commandIdx == b.commandIdx;
	}

	void CommandQueue::addBreakpoint(UINT32 queueIdx, UINT32 commandIdx)
	{
		CM_LOCK_MUTEX(CommandQueueBreakpointMutex);

		SetBreakpoints.insert(QueueBreakpoint(queueIdx, commandIdx));
	}

	void CommandQueue::breakIfNeeded(UINT32 queueIdx, UINT32 commandIdx)
	{
		// I purposely don't use a mutex here, as this gets called very often. Generally breakpoints
		// will only be added at the start of the application, so race conditions should not occur.
		auto iterFind = SetBreakpoints.find(QueueBreakpoint(queueIdx, commandIdx));

		if(iterFind != SetBreakpoints.end())
		{
			assert(false && "Command queue breakpoint triggered!");
		}
	}
#else
	void CommandQueue::addBreakpoint(UINT32 queueIdx, UINT32 commandIdx)
	{
		// Do nothing, no breakpoints in release
	}
#endif
}