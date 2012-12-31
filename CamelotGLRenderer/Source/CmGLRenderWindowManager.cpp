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

	void GLRenderWindowManager::createImpl(const RENDER_WINDOW_DESC& desc, AsyncOp& asyncOp)
	{
		GLSupport* glSupport = mRenderSystem->getGLSupport();

		// Create the window
		RenderWindow* win = glSupport->newWindow(desc);

		mRenderSystem->attachRenderTarget(*win);
		GLContext* context;
		win->getCustomAttribute("GLCONTEXT", &context);
		mRenderSystem->registerContext(context);

		RenderWindowPtr winPtr(win);
		asyncOp.completeOperation(winPtr);
	}
}