#include "CmGLRenderWindowManager.h"
#include "CmGLRenderSystem.h"
#include "CmGLSupport.h"
#include "CmAsyncOp.h"

namespace CamelotEngine
{
	GLRenderWindowManager::GLRenderWindowManager(GLRenderSystem* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	RenderWindowPtr GLRenderWindowManager::createImpl(const RENDER_WINDOW_DESC& desc)
	{
		GLSupport* glSupport = mRenderSystem->getGLSupport();

		// Create the window
		RenderWindow* win = glSupport->newWindow(desc);

		RenderWindowPtr winPtr(win);
		return winPtr;
	}
}