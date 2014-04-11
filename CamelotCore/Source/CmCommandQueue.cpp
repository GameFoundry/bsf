#include "CmCommandQueue.h"
#include "CmException.h"
#include "CmCoreThread.h"
#include "CmDebug.h"
#include "CmUtil.h"

namespace CamelotFramework
{
#if CM_DEBUG_MODE
	CommandQueueBase::CommandQueueBase(CM_THREAD_ID_TYPE threadId)
		:mMyThreadId(threadId), mMaxDebugIdx(0)
	{
		mCommands = cm_new<CamelotFramework::Queue<QueuedCommand>::type, PoolAlloc>();

		{
			CM_LOCK_MUTEX(CommandQueueBreakpointMutex);

			mCommandQueueIdx = MaxCommandQueueIdx++;
		}
	}
#else
	CommandQueueBase::CommandQueueBase(CM_THREAD_ID_TYPE threadId)
		:mMyThreadId(threadId)
	{
		mCommands = cm_new<CamelotFramework::Queue<QueuedCommand>::type, PoolAlloc>();
	}
#endif

	CommandQueueBase::~CommandQueueBase()
	{
		if(mCommands != nullptr)
			cm_delete(mCommands);

		while(!mEmptyCommandQueues.empty())
		{
			cm_delete(mEmptyCommandQueues.top());
			mEmptyCommandQueues.pop();
		}
	}

	AsyncOp CommandQueueBase::queueReturn(std::function<void(AsyncOp&)> commandCallback, bool _notifyWhenComplete, UINT32 _callbackId)
	{
#if CM_DEBUG_MODE
		breakIfNeeded(mCommandQueueIdx, mMaxDebugIdx);

		QueuedCommand newCommand(commandCallback, mMaxDebugIdx++, _notifyWhenComplete, _callbackId);
#else
		QueuedCommand newCommand(commandCallback, _notifyWhenComplete, _callbackId);
#endif

		mCommands->push(newCommand);

#if CM_FORCE_SINGLETHREADED_RENDERING
		queue<QueuedCommand>::type* commands = flush();
		playback(commands);
#endif

		return *newCommand.asyncOp;
	}

	void CommandQueueBase::queue(std::function<void()> commandCallback, bool _notifyWhenComplete, UINT32 _callbackId)
	{
#if CM_DEBUG_MODE
		breakIfNeeded(mCommandQueueIdx, mMaxDebugIdx);

		QueuedCommand newCommand(commandCallback, mMaxDebugIdx++, _notifyWhenComplete, _callbackId);
#else
		QueuedCommand newCommand(commandCallback, _notifyWhenComplete, _callbackId);
#endif

		mCommands->push(newCommand);

#if CM_FORCE_SINGLETHREADED_RENDERING
		queue<QueuedCommand>::type* commands = flush();
		playback(commands);
#endif
	}

	CamelotFramework::Queue<QueuedCommand>::type* CommandQueueBase::flush()
	{
		CamelotFramework::Queue<QueuedCommand>::type* oldCommands = mCommands;

		if(!mEmptyCommandQueues.empty())
		{
			mCommands = mEmptyCommandQueues.top();
			mEmptyCommandQueues.pop();
		}
		else
		{
			mCommands = cm_new<CamelotFramework::Queue<QueuedCommand>::type, PoolAlloc>();
		}

		return oldCommands;
	}

	void CommandQueueBase::playbackWithNotify(CamelotFramework::Queue<QueuedCommand>::type* commands, std::function<void(UINT32)> notifyCallback)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(commands == nullptr)
			return;

		while(!commands->empty())
		{
			QueuedCommand& command = commands->front();

			if(command.returnsValue)
			{
				AsyncOp& op = *command.asyncOp;
				command.callbackWithReturnValue(op);

				if(!command.asyncOp->hasCompleted())
				{
					LOGDBG("Async operation return value wasn't resolved properly. Resolving automatically to nullptr. " \
						"Make sure to complete the operation before returning from the command callback method.");
					command.asyncOp->completeOperation(nullptr);
				}
			}
			else
			{
				command.callback();
			}

			if(command.notifyWhenComplete && notifyCallback != nullptr)
			{
				notifyCallback(command.callbackId);
			}

			commands->pop();
		}

		mEmptyCommandQueues.push(commands);
	}

	void CommandQueueBase::playback(CamelotFramework::Queue<QueuedCommand>::type* commands)
	{
		playbackWithNotify(commands, std::function<void(UINT32)>());
	}

	void CommandQueueBase::cancelAll()
	{
		CamelotFramework::Queue<QueuedCommand>::type* commands = flush();

		while(!commands->empty())
			commands->pop();

		mEmptyCommandQueues.push(commands);
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

	UnorderedSet<CommandQueueBase::QueueBreakpoint, CommandQueueBase::QueueBreakpoint::HashFunction, 
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