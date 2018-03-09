//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "CoreThread/BsCoreThreadQueue.h"
#include "CoreThread/BsCommandQueue.h"
#include "Material/BsMaterial.h"
#include "CoreThread/BsCoreThread.h"

namespace bs
{
	CoreThreadQueueBase::CoreThreadQueueBase(CommandQueueBase* commandQueue)
		:mCommandQueue(commandQueue)
	{

	}

	CoreThreadQueueBase::~CoreThreadQueueBase()
	{
		bs_delete(mCommandQueue);
	}

	AsyncOp CoreThreadQueueBase::queueReturnCommand(std::function<void(AsyncOp&)> commandCallback)
	{
		return mCommandQueue->queueReturn(commandCallback);
	}

	void CoreThreadQueueBase::queueCommand(std::function<void()> commandCallback)
	{
		mCommandQueue->queue(commandCallback);
	}

	void CoreThreadQueueBase::submitToCoreThread(bool blockUntilComplete)
	{
		Queue<QueuedCommand>* commands = mCommandQueue->flush();

		gCoreThread().queueCommand(std::bind(&CommandQueueBase::playback, mCommandQueue, commands), 
			CTQF_InternalQueue | CTQF_BlockUntilComplete);
	}

	void CoreThreadQueueBase::cancelAll()
	{
		// Note that this won't free any Frame data allocated for all the canceled commands since
		// frame data will only get cleared at frame start
		mCommandQueue->cancelAll();
	}
}