#pragma once

#include "CmPrerequisites.h"
#include "CmAsyncOp.h"
#include "CmCommonEnums.h"
#include "boost/function.hpp"
#include <functional>

namespace CamelotFramework
{
	/**
	 * @brief	Contains a list of commands that can be queued by one thread,
	 * 			and executed by another.
	 */
	class CM_EXPORT CommandQueue
	{
	public:
		struct Command
		{
#ifdef CM_DEBUG_MODE
			Command(boost::function<void(AsyncOp&)> _callback, UINT32 _debugId, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
				:callbackWithReturnValue(_callback), debugId(_debugId), returnsValue(true), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId)
			{ }

			Command(boost::function<void()> _callback, UINT32 _debugId, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
				:callback(_callback), debugId(_debugId), returnsValue(false), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId)
			{ }

			UINT32 debugId;
#else
			Command(boost::function<void(AsyncOp&)> _callback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
				:callbackWithReturnValue(_callback), returnsValue(true), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId)
			{ }

			Command(boost::function<void()> _callback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
				:callback(_callback), returnsValue(false), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId)
			{ }
#endif

			boost::function<void()> callback;
			boost::function<void(AsyncOp&)> callbackWithReturnValue;
			AsyncOp asyncOp;
			bool returnsValue;
			UINT32 callbackId;
			bool notifyWhenComplete;
		};

		/**
		 * @brief	Constructor.
		 *
		 * @param	threadId	   	Identifier for the thread the command queue will be used on.
		 * @param	allowAllThreads	Only matters for debug purposes. If false, then the queue
		 * 							will throw an exception if accessed outside of the creation thread
		 * 							(If in debug mode).
		 * 							
		 *							When you want to allow multiple threads to access it, set this to true,
		 *							and also make sure you sync access to the queue properly.
		 *							
		 */
		CommandQueue(CM_THREAD_ID_TYPE threadId, bool allowAllThreads = false);
		~CommandQueue();

		CM_THREAD_ID_TYPE getThreadId() const { return mMyThreadId; }

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
		AsyncOp queueReturn(boost::function<void(AsyncOp&)> commandCallback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0);

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
		void queue(boost::function<void()> commandCallback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0);

		/**
		 * @brief	Returns a copy of all queued commands and makes room for new ones. Must be called from the thread
		 * 			that created the command queue. Returned commands MUST be passed to playbackCommands.
		 */
		std::queue<Command>* flush();

		/**
		 * @brief	Cancels all currently queued commands.
		 */
		void cancelAll();

		/**
		 * @brief	Plays all provided commands. Should only be called from the render thread. To get the
		 * 			commands call flushCommands().
		 *
		 * @param	notifyCallback  	Callback that will be called if a command that has "notifyOnComplete" flag set.
		 * 								The callback will receive "callbackId" of the command.
		 */
		void playback(std::queue<Command>* commands, boost::function<void(UINT32)> notifyCallback);

		/**
		 * @brief	Plays all provided commands. Should only be called from the render thread. To get
		 * 			the commands call flushCommands().
		 */
		void playback(std::queue<Command>* commands);

		/**
		 * @brief	Returns true if no commands are queued.
		 */
		bool isEmpty();

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

	private:
		std::queue<Command>* mCommands;

		bool mAllowAllThreads;
		
		CM_THREAD_ID_TYPE mMyThreadId;
		CM_MUTEX(mCommandBufferMutex);

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
		static std::unordered_set<QueueBreakpoint, QueueBreakpoint::HashFunction, QueueBreakpoint::EqualFunction> SetBreakpoints;
		CM_STATIC_MUTEX(CommandQueueBreakpointMutex);

		static void breakIfNeeded(UINT32 queueIdx, UINT32 commandIdx);
#endif
	};
}
