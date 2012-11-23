#include "CmRenderCommandBuffer.h"
#include "CmRenderSystem.h"
#include "CmRenderer.h"
#include "CmApplication.h"
#include "CmRenderSystemManager.h"
#include "CmRendererManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	RenderCommandBuffer::RenderCommandBuffer(CM_THREAD_ID_TYPE threadId)
	{
		mMyThreadId = threadId;
		mRenderCommands = new vector<RenderCommand>::type();
	}

	RenderCommandBuffer::~RenderCommandBuffer()
	{
		mActivePass = nullptr;
		mActiveParameters = nullptr;

		if(mRenderCommands != nullptr)
			delete mRenderCommands;
	}

	void RenderCommandBuffer::render(RenderOperation renderOp)
	{
		CM_LOCK_MUTEX(mCommandBufferMutex)

		throwIfInvalidThread();

		RenderCommand newCommand(renderOp, mActivePass, mActiveParameters);

		mRenderCommands->push_back(newCommand);
	}

	void RenderCommandBuffer::applyPass(PassPtr pass, PassParametersPtr passParameters)
	{
		CM_LOCK_MUTEX(mCommandBufferMutex)

		throwIfInvalidThread();

		mActivePass = pass;
		mActiveParameters = passParameters;
	}

	void RenderCommandBuffer::playDeferredCommands()
	{
		vector<RenderCommand>::type* oldRenderCommands = nullptr;
		
		{
			CM_LOCK_MUTEX(mCommandBufferMutex)
			oldRenderCommands = mRenderCommands;
			mRenderCommands = new vector<RenderCommand>::type();
		}

		RenderSystem* renderSystem = RenderSystemManager::getActive();
		RendererPtr renderer = RendererManager::getActive();
		
		for(auto iter = oldRenderCommands->begin(); iter != oldRenderCommands->end(); ++iter)
		{
			renderer->setPass(iter->getPass());
			renderer->setPassParameters(iter->getPassParameters());

			renderSystem->render(iter->getRenderOperation());
		}

		if(oldRenderCommands != nullptr)
			delete oldRenderCommands;

		mActivePass = nullptr;
		mActiveParameters = nullptr;
	}

	void RenderCommandBuffer::throwIfInvalidThread()
	{
		if(mMyThreadId != CM_THREAD_CURRENT_ID)
			CM_EXCEPT(InternalErrorException, "Calling the rendering context from an invalid thread.");
	}
}