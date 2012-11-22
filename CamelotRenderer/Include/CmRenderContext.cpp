#include "CmRenderContext.h"
#include "CmRenderSystem.h"
#include "CmRenderer.h"
#include "CmApplication.h"
#include "CmRenderSystemManager.h"
#include "CmRendererManager.h"

namespace CamelotEngine
{
	void RenderContext::render(RenderOperation renderOp)
	{
		RenderCommand newCommand(renderOp, mActivePass, mActiveParameters);

		mRenderCommands.push_back(newCommand);
	}

	void RenderContext::applyPass(PassPtr pass, PassParametersPtr passParameters)
	{
		mActivePass = pass;
		mActiveParameters = passParameters;
	}

	void RenderContext::playDeferredCommands()
	{
		RenderSystem* renderSystem = RenderSystemManager::getActive();
		RendererPtr renderer = RendererManager::getActive();
		
		for(auto iter = mRenderCommands.begin(); iter != mRenderCommands.end(); ++iter)
		{
			renderer->setPass(iter->getPass());
			renderer->setPassParameters(iter->getPassParameters());

			renderSystem->_render(iter->getRenderOperation());
		}

		mRenderCommands.clear();
	}
}