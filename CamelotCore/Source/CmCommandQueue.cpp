#include "CmCommandQueue.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmDebug.h"
#include "CmUtil.h"

namespace CamelotFramework
{
#if CM_DEBUG_MODE
	CommandQueueBase::CommandQueueBase(CM_THREAD_ID_TYPE threadId, bool allowAllThreads)
		:mMyThreadId(threadId), mAllowAllThreads(allowAllThreads), mMaxDebugIdx(0)
	{
		mCommands = cm_new<std::queue<QueuedCommand>, PoolAlloc>();

		{
			CM_LOCK_MUTEX(CommandQueueBreakpointMutex);

			mCommandQueueIdx = MaxCommandQueueIdx++;
		}
	}
#else
	CommandQueueBase::CommandQueueBase(CM_THREAD_ID_TYPE threadId, bool allowAllThreads)
		:mMyThreadId(threadId), mAllowAllThreads(allowAllThreads)
	{
		mCommands = cm_new<std::queue<QueuedCommand>, PoolAlloc>();
	}
#endif

	CommandQueueBase::~CommandQueueBase()
	{
		if(mCommands != nullptr)
			cm_delete(mCommands);
	}

	AsyncOp CommandQueueBase::queueReturn(boost::function<void(AsyncOp&)> commandCallback, bool _notifyWhenComplete, UINT32 _callbackId)
	{
#if CM_DEBUG_MODE
		breakIfNeeded(mCommandQueueIdx, mMaxDebugIdx);

		QueuedCommand newCommand(commandCallback, mMaxDebugIdx++, _notifyWhenComplete, _callbackId);
#else
		QueuedCommand newCommand(commandCallback, _notifyWhenComplete, _callbackId);
#endif

		mCommands->push(newCommand);

#if CM_FORCE_SINGLETHREADED_RENDERING
		std::queue<QueuedCommand>* commands = flush();
		playback(commands);
#endif

		return newCommand.asyncOp;
	}

	void CommandQueueBase::queue(boost::function<void()> commandCallback, bool _notifyWhenComplete, UINT32 _callbackId)
	{
#if CM_DEBUG_MODE
		breakIfNeeded(mCommandQueueIdx, mMaxDebugIdx);

		QueuedCommand newCommand(commandCallback, mMaxDebugIdx++, _notifyWhenComplete, _callbackId);
#else
		QueuedCommand newCommand(commandCallback, _notifyWhenComplete, _callbackId);
#endif

		mCommands->push(newCommand);

#if CM_FORCE_SINGLETHREADED_RENDERING
		std::queue<QueuedCommand>* commands = flush();
		playback(commands);
#endif
	}

	std::queue<QueuedCommand>* CommandQueueBase::flush()
	{
		std::queue<QueuedCommand>* oldCommands = mCommands;
		mCommands = cm_new<std::queue<QueuedCommand>, PoolAlloc>();

		return oldCommands;
	}

	void CommandQueueBase::playback(std::queue<QueuedCommand>* commands, boost::function<void(UINT32)> notifyCallback)
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
			QueuedCommand& command = commands->front();

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

		CM_DELETE(commands, queue<QueuedCommand>, PoolAlloc);
	}

	void CommandQueueBase::playback(std::queue<QueuedCommand>* commands)
	{
		playback(commands, boost::function<void(UINT32)>());
	}

	void CommandQueueBase::cancelAll()
	{
		std::queue<QueuedCommand>* commands = flush();
		CM_DELETE(commands, queue<QueuedCommand>, PoolAlloc);
	}

	bool CommandQueueBase::isEmpty()
	{
		if(mCommands != nullptr && mCommands->size() > 0)
			return false;

		return true;
	}

	void CommandQueueBase::throwInvalidThreadException(const String& message) const
	{
		CM_EXCEPT(InternalErrorException, message);
	}

#if CM_DEBUG_MODE
	CM_STATIC_MUTEX_CLASS_INSTANCE(CommandQueueBreakpointMutex, CommandQueueBase);

	UINT32 CommandQueueBase::MaxCommandQueueIdx = 0;

	unordered_set<CommandQueueBase::QueueBreakpoint, CommandQueueBase::QueueBreakpoint::HashFunction, 
		CommandQueueBase::QueueBreakpoint::EqualFunction>::type CommandQueueBase::SetBreakpoints;

	inline size_t CommandQueueBase::QueueBreakpoint::HashFunction::operator()(const QueueBreakpoint& v) const
	{
		size_t seed = 0;
		hash_combine(seed, v.queueIdx);
		hash_combine(seed, v.commandIdx);
		return seed;
	}

	inline bool CommandQueueBase::QueueBreakpoint::EqualFunction::operator()(const QueueBreakpoint &a, const QueueBreakpoint &b) const
	{
		return a.queueIdx == b.queueIdx && a.commandIdx == b.commandIdx;
	}

	void CommandQueueBase::addBreakpoint(UINT32 queueIdx, UINT32 commandIdx)
	{
		CM_LOCK_MUTEX(CommandQueueBreakpointMutex);

		SetBreakpoints.insert(QueueBreakpoint(queueIdx, commandIdx));
	}

	void CommandQueueBase::breakIfNeeded(UINT32 queueIdx, UINT32 commandIdx)
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
	void CommandQueueBase::addBreakpoint(UINT32 queueIdx, UINT32 commandIdx)
	{
		// Do nothing, no breakpoints in release
	}
#endif
}