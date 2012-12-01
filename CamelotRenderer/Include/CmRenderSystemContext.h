#pragma once

#include "CmPrerequisites.h"
#include "CmAsyncOp.h"
#include "CmCommon.h"
#include "boost/function.hpp"

namespace CamelotEngine
{
	/**
	 * @brief	Contains context specific information for a render system.
	 * 			Usually each thread will have its own RenderSystemContext, 
	 * 			but you are allowed to have multiple contexts per thread.
	 * 			Contexts can't be shared between threads though.
	 */
	class RenderSystemContext
	{
	public:
		CM_THREAD_ID_TYPE getThreadId() const { return mMyThreadId; }

	protected:
		struct RenderSystemCommand
		{
			RenderSystemCommand(boost::function<void(AsyncOp&)> _callback, UINT32 _callbackId = 0)
				:callbackWithReturnValue(_callback), returnsValue(true), callbackId(_callbackId)
			{ }

			RenderSystemCommand(boost::function<void()> _callback, UINT32 _callbackId = 0)
				:callback(_callback), returnsValue(false), callbackId(_callbackId)
			{ }

			boost::function<void()> callback;
			boost::function<void(AsyncOp&)> callbackWithReturnValue;
			AsyncOp asyncOp;
			bool returnsValue;
			UINT32 callbackId;
		};

		/************************************************************************/
		/* 					CALLABLE ONLY FROM RENDERSYSTEM                     */
		/************************************************************************/
		friend class RenderSystem;

		// Actively being filled up
		vector<RenderSystemCommand>::type* mCommands;

		bool mIsShutdown;
		bool mIsExecuting;

		CM_THREAD_ID_TYPE mMyThreadId;
		CM_MUTEX(mCommandBufferMutex)
		CM_THREAD_SYNCHRONISER(mContextPlaybackDoneCondition)

		RenderSystemContext(CM_THREAD_ID_TYPE threadId);

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
		 * @param	_callbackId			   	(optional) Identifier for the callback so you can then later find it
		 * 									if needed.
		 *
		 * @return	Async operation object you can continuously check until the command completes. After
		 * 			it completes AsyncOp::isResolved will return true and return data will be valid (if
		 * 			the callback provided any).
		 */
		virtual AsyncOp queueReturnCommand(boost::function<void(AsyncOp&)> commandCallback, UINT32 _callbackId = 0) = 0;

		/**
		 * @brief	Queue up a new command to execute. Make sure the provided function has all of its
		 * 			parameters properly bound. Provided command is not expected to return a value.
		 * 			If you wish to return a value from the callback use the other overload of queueCommand 
		 * 			which accepts AsyncOp parameter.
		 *
		 * @param	_callbackId			   	(optional) Identifier for the callback so you can then later find it
		 * 									if needed.
		 */
		virtual void queueCommand(boost::function<void()> commandCallback, UINT32 _callbackId = 0) = 0;

		/**
		 * @brief	Plays all queued commands. Should only be called from the render thread,
		 * 			and is normally called by the RenderSystem internally.
		 */
		virtual void playbackCommands() = 0;

		/**
		 * @brief	Query if this object has any commands ready for rendering.
		 */
		virtual bool hasReadyCommands() = 0;

		/**
		 * @brief	Makes all the currently queued commands available to the GPU. They will be executed
		 * 			as soon as the render thread is ready.
		 * 			
		 * @note	This is expected to be called once per frame. If the previous set of commands hasn't even started rendering
		 * 			yet, it will be discarded. This is to prevent lag if the simulation executes faster than the render thread.
		 */
		virtual void submitToGpu() = 0;

		/**
		 * @brief	Blocks the current thread until all commands in the context are processed.
		 * 			
		 * @note	Do not call from render thread. Render thread is the thread doing the processing and blocking
		 * 			it will cause a deadlock since processing will never be completed. 
		 */
		virtual void blockUntilExecuted() = 0;

		/************************************************************************/
		/* 								STATES		                     		*/
		/* These are various states we cache per RenderSystemContext. They are  */
		/* used as a quick way of accessing the current state. If we didn't keep*/
		/* these, then whenever user wanted to know a certain state we would    */
		/* have to go and execute all commands to find what it is.				*/
		/************************************************************************/

		// TODO - This is actually a clumsy way of keeping state. Better and more generic way would be to
		// search through all commands for a specific state change and find what it is.
		bool waitForVerticalBlank;
		CullingMode cullingMode;
		bool vertexProgramBound;
        bool geometryProgramBound;
		bool fragmentProgramBound;
		bool invertVertexWinding;
	};

	/**
	 * @brief	Render system context where commands are added and meant to be executed once per frame.
	 * 			Obsolete commands get discarded as new ones are submitted.
	 */
	class RenderSystemFrameContext : public RenderSystemContext
	{
	protected:
		/************************************************************************/
		/* 					CALLABLE ONLY FROM RENDERSYSTEM                     */
		/************************************************************************/
		friend class RenderSystem;

		// Finalized and ready for rendering
		vector<RenderSystemCommand>::type* mReadyCommands;

		RenderSystemFrameContext(CM_THREAD_ID_TYPE threadId);

		/**
		 * @copydoc RenderSystemContext::queueReturnCommand()
		 */
		AsyncOp queueReturnCommand(boost::function<void(AsyncOp&)> commandCallback, UINT32 _callbackId = 0);

		 /**
		 * @copydoc RenderSystemContext::queueCommand()
		 */
		void queueCommand(boost::function<void()> commandCallback, UINT32 _callbackId = 0);

		 /**
		 * @copydoc RenderSystemContext::playbackCommands()
		 */
		void playbackCommands();

		 /**
		 * @copydoc RenderSystemContext::hasReadyCommands()
		 */
		bool hasReadyCommands();

		/**
		 * @copydoc RenderSystemContext::submitToGpu()
		 */
		void submitToGpu();

		/**
		 * @copydoc RenderSystemContext::blockUntilExecuted()
		 */
		void blockUntilExecuted();
	};

	/**
	 * @brief	Render system context where commands are added and scheduled for execution immediately.
	 */
	class RenderSystemImmediateContext : public RenderSystemContext
	{
	protected:
		/************************************************************************/
		/* 					CALLABLE ONLY FROM RENDERSYSTEM                     */
		/************************************************************************/
		friend class RenderSystem;

		RenderSystemImmediateContext(CM_THREAD_ID_TYPE threadId);

		/**
		 * @copydoc RenderSystemContext::queueReturnCommand()
		 */
		AsyncOp queueReturnCommand(boost::function<void(AsyncOp&)> commandCallback, UINT32 _callbackId = 0);

		 /**
		 * @copydoc RenderSystemContext::queueCommand()
		 */
		void queueCommand(boost::function<void()> commandCallback, UINT32 _callbackId = 0);

		 /**
		 * @copydoc RenderSystemContext::playbackCommands()
		 */
		void playbackCommands();

		 /**
		 * @copydoc RenderSystemContext::hasReadyCommands()
		 */
		bool hasReadyCommands();

		/**
		 * @copydoc RenderSystemContext::submitToGpu()
		 */
		void submitToGpu();

		/**
		 * @copydoc RenderSystemContext::blockUntilExecuted()
		 */
		void blockUntilExecuted();
	};
}
