#pragma once

#include "CmPrerequisites.h"
#include "CmAsyncOp.h"
#include "CmCommonEnums.h"
#include <functional>

namespace BansheeEngine
{
	class CommandQueueNoSync
	{
	public:
		CommandQueueNoSync() {}
		virtual ~CommandQueueNoSync() {}

		bool isValidThread(CM_THREAD_ID_TYPE ownerThread) const
		{
			return CM_THREAD_CURRENT_ID == ownerThread;
		}

		void lock() 
		{
		};

		void unlock()
		{
		}

	};

	class CommandQueueSync
	{
	public:
		CommandQueueSync()
			:mLock(mCommandQueueMutex, CM_DEFER_LOCK)
		{ }
		virtual ~CommandQueueSync() {}

		bool isValidThread(CM_THREAD_ID_TYPE ownerThread) const
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
		CM_MUTEX(mCommandQueueMutex);
		CM_LOCK_TYPE mLock;
	};

	struct QueuedCommand
	{
#if CM_DEBUG_MODE
		QueuedCommand(std::function<void(AsyncOp&)> _callback, UINT32 _debugId, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
			:callbackWithReturnValue(_callback), debugId(_debugId), returnsValue(true), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId), asyncOp(cm_new<AsyncOp>()), ownsData(true)
		{ }

		QueuedCommand(std::function<void()> _callback, UINT32 _debugId, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
			:callback(_callback), debugId(_debugId), returnsValue(false), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId), asyncOp(nullptr), ownsData(true)
		{ }

		UINT32 debugId;
#else
		QueuedCommand(std::function<void(AsyncOp&)> _callback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
			:callbackWithReturnValue(_callback), returnsValue(true), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId), asyncOp(cm_new<AsyncOp>()), ownsData(true)
		{ }

		QueuedCommand(std::function<void()> _callback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
			:callback(_callback), returnsValue(false), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId), asyncOp(nullptr), ownsData(true)
		{ }
#endif

		~QueuedCommand()
		{
			if(ownsData && asyncOp != nullptr)
				cm_delete(asyncOp);
		}

		QueuedCommand(const QueuedCommand& source)
		{
			ownsData = true;
			source.ownsData = false;

			callback = source.callback;
			callbackWithReturnValue = source.callbackWithReturnValue;
			asyncOp = source.asyncOp;
			returnsValue = source.returnsValue;
			callbackId = source.callbackId;
			notifyWhenComplete = source.notifyWhenComplete;
		}

		QueuedCommand& operator=(const QueuedCommand& rhs)
		{
			ownsData = true;
			rhs.ownsData = false;

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
		AsyncOp* asyncOp;
		bool returnsValue;
		UINT32 callbackId;
		bool notifyWhenComplete;

		mutable bool ownsData;
	};

	/**
	 * @brief	Contains a list of commands that can be queued by one thread,
	 * 			and executed by another.
	 */
	class CM_EXPORT CommandQueueBase
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	threadId	   	Identifier for the thread the command queue will be used on.						
		 */
		CommandQueueBase(CM_THREAD_ID_TYPE threadId);
		virtual ~CommandQueueBase();

		CM_THREAD_ID_TYPE getThreadId() const { return mMyThreadId; }

		/**
		 * @brief	Plays all provided commands. To get the commands call flush().
		 *
		 * @param	notifyCallback  	Callback that will be called if a command that has "notifyOnComplete" flag set.
		 * 								The callback will receive "callbackId" of the command.
		 */
		void playbackWithNotify(Queue<QueuedCommand>::type* commands, std::function<void(UINT32)> notifyCallback);

		/**
		 * @brief	Plays all provided commands. To get the commands call flush().
		 */
		void playback(Queue<QueuedCommand>::type* commands);

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

	protected:
		/**
		 * @brief	Queue up a new command to execute. Make sure the provided function has all of its
		 * 			parameters properly bound. Last parameter must be unbound and of AsyncOp&amp; type.
		 * 			This is used to signal that the command is completed, and also for storing the return
		 * 			value.
		 * 			
		 * 			@note	Callback method also needs to call AsyncOp::markAsResolved once it is done
		 * 			processing. (If it doesn't it will still be called automatically, but the return
		 * 			value will default to nullptr)
		 *
		 * @param	_notifyWhenComplete	(optional) Call the notify method (provided in the call to CommandQueue::playback)
		 * 								when the command is complete.
		 * @param	_callbackId			   	(optional) Identifier for the callback so you can then later find it
		 * 									if needed.
		 *
		 * @return	Async operation object you can continuously check until the command completes. After
		 * 			it completes AsyncOp::isResolved will return true and return data will be valid (if
		 * 			the callback provided any).
		 */
		AsyncOp queueReturn(std::function<void(AsyncOp&)> commandCallback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0);

		/**
		 * @brief	Queue up a new command to execute. Make sure the provided function has all of its
		 * 			parameters properly bound. Provided command is not expected to return a value. If you
		 * 			wish to return a value from the callback use the other overload of queueCommand which
		 * 			accepts AsyncOp parameter.
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
		BansheeEngine::Queue<QueuedCommand>::type* flush();

		/**
		 * @brief	Cancels all currently queued commands.
		 */
		void cancelAll();

		/**
		 * @brief	Returns true if no commands are queued.
		 */
		bool isEmpty();

	protected:
		void throwInvalidThreadException(const String& message) const;

	private:
		BansheeEngine::Queue<QueuedCommand>::type* mCommands;
		Stack<BansheeEngine::Queue<QueuedCommand>::type*>::type mEmptyCommandQueues; // List of empty queues for reuse

		CM_THREAD_ID_TYPE mMyThreadId;

		// Various variables that allow for easier debugging by allowing us to trigger breakpoints
		// when a certain command was queued.
#if CM_DEBUG_MODE
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

		static UINT32 MaxCommandQueueIdx;
		static UnorderedSet<QueueBreakpoint, QueueBreakpoint::HashFunction, QueueBreakpoint::EqualFunction>::type SetBreakpoints;
		CM_STATIC_MUTEX(CommandQueueBreakpointMutex);

		static void breakIfNeeded(UINT32 queueIdx, UINT32 commandIdx);
#endif
	};

	/**
	 * @copydoc CommandQueueBase
	 */
	template<class SyncPolicy = CommandQueueNoSync>
	class CommandQueue : public CommandQueueBase, public SyncPolicy
	{
	public:
		/**
		 * @copydoc CommandQueueBase::CommandQueueBase
		 */
		CommandQueue(CM_THREAD_ID_TYPE threadId)
			:CommandQueueBase(threadId)
		{ }

		~CommandQueue() 
		{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
			if(!isValidThread(getThreadId()))
				throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
#endif
		}

		/**
		 * @copydoc CommandQueueBase::queueReturn
		 */
		AsyncOp queueReturn(std::function<void(AsyncOp&)> commandCallback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
		{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
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
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
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
		BansheeEngine::Queue<QueuedCommand>::type* flush()
		{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
			if(!isValidThread(getThreadId()))
				throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
#endif

			lock();
			BansheeEngine::Queue<QueuedCommand>::type* commands = CommandQueueBase::flush();
			unlock();

			return commands;
		}

		/**
		 * @copydoc CommandQueueBase::cancelAll
		 */
		void cancelAll()
		{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
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
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
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
