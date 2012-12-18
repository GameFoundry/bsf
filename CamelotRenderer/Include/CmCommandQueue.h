#pragma once

#include "CmPrerequisites.h"
#include "CmAsyncOp.h"
#include "CmCommon.h"
#include "boost/function.hpp"

namespace CamelotEngine
{
	/**
	 * @brief	Contains a list of commands that can be queued by one thread,
	 * 			and executed by another.
	 */
	class CommandQueue
	{
	public:
		struct Command
		{
			Command(boost::function<void(AsyncOp&)> _callback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
				:callbackWithReturnValue(_callback), returnsValue(true), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId)
			{ }

			Command(boost::function<void()> _callback, bool _notifyWhenComplete = false, UINT32 _callbackId = 0)
				:callback(_callback), returnsValue(false), notifyWhenComplete(_notifyWhenComplete), callbackId(_callbackId)
			{ }

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
		vector<Command>::type* flush();

		/**
		 * @brief	Plays all provided commands. Should only be called from the render thread. To get the
		 * 			commands call flushCommands().
		 *
		 * @param	notifyCallback  	Callback that will be called if a command that has "notifyOnComplete" flag set.
		 * 								The callback will receive "callbackId" of the command.
		 */
		void playback(vector<Command>::type* commands, boost::function<void(UINT32)> notifyCallback);

		/**
		 * @brief	Plays all provided commands. Should only be called from the render thread. To get
		 * 			the commands call flushCommands().
		 */
		void playback(vector<Command>::type* commands);

		/**
		 * @brief	Returns true if no commands are queued.
		 */
		bool isEmpty();

	private:
		vector<Command>::type* mCommands;

		bool mIsShutdown;
		bool mAllowAllThreads;

		CM_THREAD_ID_TYPE mMyThreadId;
		CM_MUTEX(mCommandBufferMutex);
	};
}
