#pragma once

#include "BsCorePrerequisites.h"
#include "BsRenderAPI.h"
#include "BsCommandQueue.h"
#include "BsAsyncOp.h"
#include "BsViewport.h"
#include "BsColor.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains some base functionality used for CoreThreadAccessor.
	 * 			
	 * @see		CoreThreadAccesor
	 */
	class BS_CORE_EXPORT CoreThreadAccessorBase
	{
	public:
		CoreThreadAccessorBase(CommandQueueBase* commandQueue);
		virtual ~CoreThreadAccessorBase();

		/**
		* @brief	Queues a new generic command that will be added to the command queue.
		*/
		AsyncOp queueReturnCommand(std::function<void(AsyncOp&)> commandCallback);

		/**
		* @brief	Queues a new generic command that will be added to the command queue.
		*/
		void queueCommand(std::function<void()> commandCallback);

		/**
		 * @brief	Makes all the currently queued commands available to the core thread. They will be executed
		 * 			as soon as the core thread is ready. All queued commands are removed from the accessor.
		 */
		void submitToCoreThread(bool blockUntilComplete = false);

		/**
		 * @brief	Cancels all commands in the queue.
		 */
		void cancelAll();

	private:
		CommandQueueBase* mCommandQueue;
	};

	/**
	 * @brief	Core thread accessor allows you to schedule core commands outside of the core thread. Provides a set of common
	 * 			methods you may want to execute on the core thread, as well as a general command queuing methods.
	 * 			
	 * @note	Queued commands are only executed after the call to submitToCoreThread, in the order they were submitted.
	 */
	template <class CommandQueueSyncPolicy = CommandQueueNoSync>
	class BS_CORE_EXPORT CoreThreadAccessor : public CoreThreadAccessorBase
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	threadId		Identifier for the thread that created the accessor.
		 */
		CoreThreadAccessor(BS_THREAD_ID_TYPE threadId)
			:CoreThreadAccessorBase(bs_new<CommandQueue<CommandQueueSyncPolicy>>(threadId))
		{

		}
	};
}