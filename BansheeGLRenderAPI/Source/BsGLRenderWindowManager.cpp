#include "BsGLRenderWindowManager.h"
#include "BsGLRenderAPI.h"
#include "BsGLSupport.h"
#include "BsAsyncOp.h"

namespace BansheeEngine
{
	GLRenderWindowManager::GLRenderWindowManager(GLRenderAPI* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	RenderWindowPtr GLRenderWindowManager::createImpl(RENDER_WINDOW_DESC& desc, UINT32 windowId, const RenderWindowPtr& parentWindow)
	{
		GLSupport* glSupport = mRenderSystem->getGLSupport();

		// Create the window
		return glSupport->newWindow(desc, windowId, parentWindow);
	}

	GLRenderWindowCoreManager::GLRenderWindowCoreManager(GLRenderAPI* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	SPtr<RenderWindowCore> GLRenderWindowCoreManager::createInternal(RENDER_WINDOW_DESC& desc, UINT32 windowId)
	{
		GLSupport* glSupport = mRenderSystem->getGLSupport();

		// Create the window
		SPtr<RenderWindowCore> window = glSupport->newWindowCore(desc, windowId);
		window->_setThisPtr(window);

		windowCreated(window.get());

		return window;
	}
}