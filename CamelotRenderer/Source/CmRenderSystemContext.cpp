#include "CmRenderSystemContext.h"
#include "CmException.h"
#include "CmRenderSystemManager.h"
#include "CmRenderSystem.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	RenderSystemContext::RenderSystemContext(CM_THREAD_ID_TYPE threadId)
		:mMyThreadId(threadId), mReadyCommands(nullptr), mIsExecuting(false)
	{
		mCommands = new vector<RenderSystemCommand>::type();
	}

	AsyncOp RenderSystemContext::queueCommand(boost::function<void(AsyncOp&)> commandCallback)
	{
#if CM_DEBUG_MODE
#if CM_THREAD_SUPPORT != 0
		if(CM_THREAD_CURRENT_ID != mMyThreadId)
		{
			CM_EXCEPT(InternalErrorException, "Render system context accessed from an invalid thread.");
		}
#endif
#endif

		RenderSystemCommand newCommand(commandCallback);
		mCommands->push_back(newCommand);

		return newCommand.asyncOp;
	}

	void RenderSystemContext::submitToGpu()
	{
		{
			CM_LOCK_MUTEX(mCommandBufferMutex);

			if(mReadyCommands != nullptr)
			{
				delete mReadyCommands;
				mReadyCommands = nullptr;
			}

			mReadyCommands = mCommands;
			mCommands = new vector<RenderSystemCommand>::type();
		}
	}

	void RenderSystemContext::playbackCommands()
	{
#if CM_DEBUG_MODE
		RenderSystem* rs = RenderSystemManager::getActive();

		if(rs->getRenderThreadId() != CM_THREAD_CURRENT_ID)
			CM_EXCEPT(InternalErrorException, "This method should only be called from the render thread.");
#endif

		vector<RenderSystemCommand>::type* currentCommands = nullptr;
		{
			CM_LOCK_MUTEX(mCommandBufferMutex)

			currentCommands = mReadyCommands;
			mReadyCommands = nullptr;
			mIsExecuting = true;
		}

		if(currentCommands == nullptr)
		{
			{
				CM_LOCK_MUTEX(mCommandBufferMutex);
				mIsExecuting = false;
			}

			CM_THREAD_NOTIFY_ALL(mContextPlaybackDoneCondition)
			return;
		}

		for(auto iter = currentCommands->begin(); iter != currentCommands->end(); ++iter)
		{
			(*iter).callback((*iter).asyncOp);

			if(!(*iter).asyncOp.hasCompleted())
			{
				LOGDBG("Async operation wasn't resolved properly. Resolving automatically to nullptr.");
				(*iter).asyncOp.completeOperation(nullptr);
			}
		}

		delete currentCommands;

		{
			CM_LOCK_MUTEX(mCommandBufferMutex);
			mIsExecuting = false;
		}

		CM_THREAD_NOTIFY_ALL(mContextPlaybackDoneCondition)
	}

	bool RenderSystemContext::hasReadyCommands()
	{
		CM_LOCK_MUTEX(mCommandBufferMutex);

		if(mReadyCommands != nullptr && mReadyCommands->size() > 0)
			return true;

		return false;
	}

	void RenderSystemContext::blockUntilExecuted()
	{
#if CM_DEBUG_MODE
		RenderSystem* rs = RenderSystemManager::getActive();

		if(rs->getRenderThreadId() == CM_THREAD_CURRENT_ID)
			CM_EXCEPT(InternalErrorException, "This method should never be called from the render thread as it will cause a deadlock.");
#endif

		{
			CM_LOCK_MUTEX_NAMED(mCommandBufferMutex, lock);
			while (mReadyCommands != nullptr && mReadyCommands->size() > 0 || mIsExecuting)
			{
				CM_THREAD_WAIT(mContextPlaybackDoneCondition, mCommandBufferMutex, lock)
			}
		}
	}
}