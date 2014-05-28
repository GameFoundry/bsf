#include "CmGLRenderWindowManager.h"
#include "CmGLRenderSystem.h"
#include "CmGLSupport.h"
#include "CmAsyncOp.h"

namespace BansheeEngine
{
	GLRenderWindowManager::GLRenderWindowManager(GLRenderSystem* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	RenderWindowPtr GLRenderWindowManager::createImpl(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		GLSupport* glSupport = mRenderSystem->getGLSupport();

		// Create the window
		return glSupport->newWindow(desc, parentWindow);
	}
}