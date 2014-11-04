#pragma once

#include "BsCorePrerequisites.h"
#include "BsAsyncOp.h"
#include <functional>

namespace BansheeEngine
{
	/**
	 * @brief	Command queue policy that provides no synchonization. Should be used
	 * 			with command queues that are used on a single thread only.
	 */
	class CommandQueueNoSync
	{
	public:
		CommandQueueNoSync() {}
		virtual ~CommandQueueNoSync() {}

		bool isValidThread(BS_THREAD_ID_TYPE ownerThread) const
		{
			return BS_THREAD_CURRENT_ID == ownerThread;
		}

		void lock() { };
		void unlock() { }
	};

	/**
	 * @brief	Command queue policy that provides synchonization. Should be used
	 * 			with command queues that are used on multiple threads.
	 */
	class CommandQueueSync
	{
	public:
		CommandQueueSync()
			:mLock(mCommandQueueMutex, BS_DEFER_LOCK)
		{ }
		virtual ~CommandQueueSync() {}

		bool isValidThread(BS_THREAD_ID_TYPE ownerThread) const
		{
			return true;
		}

		void lock() 
		{
			mLock.lock();
		};

		void unlock()
		{
			mLock.unlock();
		}

	private:
		BS_MUTEX(mCommandQueueMutex);
		BS_LOCK_TYPE mLock;
	};

	/**
	 * @brief	Represents a single queued command in the command list. Contains all the data for executing the command
	 * 			and checking up on the command status.
	 */
	struct QueuedCommand
	{
#if BS_DEBUG_MODE
		QueuedCommand(std::function<void(AsyncOp&)> _callback, UINT32 _debugId, const AsyncOpSyncDataPtr& asyncOpSyncData,
			bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
			:callbackWithReturnValue(_callback), debugId(_debugId), returnsValue(true), 
			notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId), asyncOp(asyncOpSyncData)
		{ }

		QueuedCommand(std::function<void()> _callback, UINT32 _debugId, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
			:callback(_callback), debugId(_debugId), returnsValue(false), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId), asyncOp(AsyncOpEmpty())
		{ }

		UINT32 debugId;
#else
		QueuedCommand(std::function<void(AsyncOp&)> _callback, const AsyncOpSyncDataPtr& asyncOpSyncData, 
			bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
			:callbackWithReturnValue(_callback), returnsValue(true), notifyWhenComplete(_notifyWhenComplete), 
			callbackId(_callbackId), asyncOp(asyncOpSyncData)
		{ }

		QueuedCommand(std::function<void()> _callback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
			:callback(_callback), returnsValue(false), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId), asyncOp(AsyncOpEmpty())
		{ }
#endif

		~QueuedCommand()
		{ }

		QueuedCommand(const QueuedCommand& source)
		{
			callback = source.callback;
			callbackWithReturnValue = source.callbackWithReturnValue;
			asyncOp = source.asyncOp;
			returnsValue = source.returnsValue;
			callbackId = source.callbackId;
			notifyWhenComplete = source.notifyWhenComplete;
		}

		QueuedCommand& operator=(const QueuedCommand& rhs)
		{
			callback = rhs.callback;
			callbackWithReturnValue = rhs.callbackWithReturnValue;
			asyncOp = rhs.asyncOp;
			returnsValue = rhs.returnsValue;
			callbackId = rhs.callbackId;
			notifyWhenComplete = rhs.notifyWhenComplete;
			
			return *this;
		}

		std::function<void()> callback;
		std::function<void(AsyncOp&)> callbackWithReturnValue;
		AsyncOp asyncOp;
		bool returnsValue;
		UINT32 callbackId;
		bool notifyWhenComplete;
	};

	/**
	 * @brief	Contains a list of commands you may queue for later execution on the core thread.
	 */
	class BS_CORE_EXPORT CommandQueueBase
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	threadId	   	Identifier for the thread the command queue will be getting commands from.					
		 */
		CommandQueueBase(BS_THREAD_ID_TYPE threadId);
		virtual ~CommandQueueBase();

		/**
		 * @brief	Gets the thread identifier the command queue is used on.
		 * 			
		 * @note	If the command queue is using a synchonized access policy generally this
		 * 			is not relevant as it may be used on multiple threads.
		 */
		BS_THREAD_ID_TYPE getThreadId() const { return mMyThreadId; }

		/**
		 * @brief	Executes all provided commands one by one in order. To get the commands you should call flush().
		 *
		 * @param	notifyCallback  	Callback that will be called if a command that has "notifyOnComplete" flag set.
		 * 								The callback will receive "callbackId" of the command.
		 */
		void playbackWithNotify(Queue<QueuedCommand>* commands, std::function<void(UINT32)> notifyCallback);

		/**
		 * @brief	Executes all provided commands one by one in order. To get the commands you should call flush().
		 */
		void playback(Queue<QueuedCommand>* commands);

		/**
		 * @brief	Allows you to set a breakpoint that will trigger when the specified command is executed.
		 * 			
		 * @note	This is helpful when you receive an error on the executing thread and you cannot tell from where was
		 * 			the command that caused the error queued from. However you can make a note of the queue and command index
		 * 			and set a breakpoint so that it gets triggered next time you run the program. At that point you can know 
		 * 			exactly which part of code queued the command by examining the stack trace.
		 *
		 * @param	queueIdx  	Zero-based index of the queue the command was queued on.
		 * @param	commandIdx	Zero-based index of the command.
		 */
		static void addBreakpoint(UINT32 queueIdx, UINT32 commandIdx);

		/**
		 * @brief	Queue up a new command to execute. Make sure the provided function has all of its
		 * 			parameters properly bound. Last parameter must be unbound and of AsyncOp& type.
		 * 			This is used to signal that the command is completed, and also for storing the return
		 * 			value.
		 * 			
		 * 			@note	Callback method also needs to call AsyncOp::markAsResolved once it is done
		 * 			processing. (If it doesn't it will still be called automatically, but the return
		 * 			value will default to nullptr)
		 *
		 * @param	_notifyWhenComplete	(optional) Call the notify method (provided in the call to CommandQueue::playback)
		 * 								when the command is complete.
		 * @param	_callbackId			(optional) Identifier for the callback so you can then later find it
		 * 								if needed.
		 *
		 * @return	Async operation object that you can continuously check until the command completes. After
		 * 			it completes AsyncOp::isResolved will return true and return data will be valid (if
		 * 			the callback provided any).
		 */
		AsyncOp queueReturn(std::function<void(AsyncOp&)> commandCallback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0);

		/**
		 * @brief	Queue up a new command to execute. Make sure the provided function has all of its
		 * 			parameters properly bound. Provided command is not expected to return a value. If you
		 * 			wish to return a value from the callback use the queueReturn which accepts an AsyncOp parameter.
		 *
		 * @param	_notifyWhenComplete	(optional) Call the notify method (provided in the call to CommandQueue::playback)
		 * 								when the command is complete.
		 * @param	_callbackId		   	(optional) Identifier for the callback so you can then later find
		 * 								it if needed.
		 */
		void queue(std::function<void()> commandCallback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0);

		/**
		 * @brief	Returns a copy of all queued commands and makes room for new ones. Must be called from the thread
		 * 			that created the command queue. Returned commands MUST be passed to "playback" method.
		 */
		BansheeEngine::Queue<QueuedCommand>* flush();

		/**
		 * @brief	Cancels all currently queued commands.
		 */
		void cancelAll();

		/**
		 * @brief	Returns true if no commands are queued.
		 */
		bool isEmpty();

	protected:
		/**
		 * @brief	Helper method that throws an "Invalid thread" exception. Used primarily
		 * 			so we can avoid including Exception include in this header.
		 */
		void throwInvalidThreadException(const String& message) const;

	private:
		BansheeEngine::Queue<QueuedCommand>* mCommands;
		Stack<BansheeEngine::Queue<QueuedCommand>*> mEmptyCommandQueues; // List of empty queues for reuse

		BS_THREAD_ID_TYPE mMyThreadId;

		// Various variables that allow for easier debugging by allowing us to trigger breakpoints
		// when a certain command was queued.
#if BS_DEBUG_MODE
		struct QueueBreakpoint
		{
			class HashFunction
			{
			public:
				size_t operator()(const QueueBreakpoint &key) const;
			};

			class EqualFunction
			{
			public:
				bool operator()(const QueueBreakpoint &a, const QueueBreakpoint &b) const;
			};

			QueueBreakpoint(UINT32 _queueIdx, UINT32 _commandIdx)
				:queueIdx(_queueIdx), commandIdx(_commandIdx)
			{ }

			UINT32 queueIdx;
			UINT32 commandIdx;

			inline size_t operator()(const QueueBreakpoint& v) const;
		};

		UINT32 mMaxDebugIdx;
		UINT32 mCommandQueueIdx;
		AsyncOpSyncDataPtr mAsyncOpSyncData;

		static UINT32 MaxCommandQueueIdx;
		static UnorderedSet<QueueBreakpoint, QueueBreakpoint::HashFunction, QueueBreakpoint::EqualFunction> SetBreakpoints;
		BS_STATIC_MUTEX(CommandQueueBreakpointMutex);

		/**
		 * @brief	Checks if the specified command has a breakpoint and throw an assert if it does.
		 */
		static void breakIfNeeded(UINT32 queueIdx, UINT32 commandIdx);
#endif
	};

	/**
	 * @copydoc CommandQueueBase
	 * 			
	 * @brief	Use SyncPolicy to choose whether you want command queue be synchonized or not. Synchonized
	 * 			command queues may be used across multiple threads and non-synchonized only on one.
	 */
	template<class SyncPolicy = CommandQueueNoSync>
	class CommandQueue : public CommandQueueBase, public SyncPolicy
	{
	public:
		/**
		 * @copydoc CommandQueueBase::CommandQueueBase
		 */
		CommandQueue(BS_THREAD_ID_TYPE threadId)
			:CommandQueueBase(threadId)
		{ }

		~CommandQueue() 
		{ }

		/**
		 * @copydoc CommandQueueBase::queueReturn
		 */
		AsyncOp queueReturn(std::function<void(AsyncOp&)> commandCallback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
		{
#if BS_DEBUG_MODE
#if BS_THREAD_SUPPORT != 0
			if(!isValidThread(getThreadId()))
				throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
#endif

			lock();
			AsyncOp asyncOp = CommandQueueBase::queueReturn(commandCallback, _notifyWhenComplete, _callbackId);
			unlock();

			return asyncOp;
		}

		/**
		 * @copydoc CommandQueueBase::queue
		 */
		void queue(std::function<void()> commandCallback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
		{
#if BS_DEBUG_MODE
#if BS_THREAD_SUPPORT != 0
			if(!isValidThread(getThreadId()))
				throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
#endif

			lock();
			CommandQueueBase::queue(commandCallback, _notifyWhenComplete, _callbackId);
			unlock();
		}

		/**
		 * @copydoc CommandQueueBase::flush
		 */
		BansheeEngine::Queue<QueuedCommand>* flush()
		{
#if BS_DEBUG_MODE
#if BS_THREAD_SUPPORT != 0
			if(!isValidThread(getThreadId()))
				throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
#endif

			lock();
			BansheeEngine::Queue<QueuedCommand>* commands = CommandQueueBase::flush();
			unlock();

			return commands;
		}

		/**
		 * @copydoc CommandQueueBase::cancelAll
		 */
		void cancelAll()
		{
#if BS_DEBUG_MODE
#if BS_THREAD_SUPPORT != 0
			if(!isValidThread(getThreadId()))
				throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
#endif

			lock();
			CommandQueueBase::cancelAll();
			unlock();
		}

		/**
		 * @copydoc CommandQueueBase::isEmpty
		 */
		bool isEmpty()
		{
#if BS_DEBUG_MODE
#if BS_THREAD_SUPPORT != 0
			if(!isValidThread(getThreadId()))
				throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
#endif

			lock();
			bool empty = CommandQueueBase::isEmpty();
			unlock();

			return empty;
		}
	};
}
