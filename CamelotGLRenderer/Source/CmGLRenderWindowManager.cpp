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

	void GLRenderWindowManager::createImpl(const String& name, UINT32 width, UINT32 height, 
		bool fullScreen, const map<String, String>::type& miscParams, AsyncOp& asyncOp)
	{
		GLSupport* glSupport = mRenderSystem->getGLSupport();

		// Create the window
		RenderWindow* win = glSupport->newWindow(name, width, height, fullScreen, &miscParams);

		mRenderSystem->attachRenderTarget(*win);
		GLContext* context;
		win->getCustomAttribute_internal("GLCONTEXT", &context);
		mRenderSystem->registerContext(context);

		RenderWindowPtr winPtr(win);
		asyncOp.completeOperation(winPtr);
	}
}