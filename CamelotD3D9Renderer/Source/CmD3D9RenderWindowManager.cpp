#include "CmD3D9RenderWindowManager.h"
#include "CmD3D9RenderSystem.h"
#include "CmD3D9RenderWindow.h"
#include "CmAsyncOp.h"

namespace CamelotEngine
{
	D3D9RenderWindowManager::D3D9RenderWindowManager(D3D9RenderSystem* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	void D3D9RenderWindowManager::createImpl(const RENDER_WINDOW_DESC& desc, AsyncOp& asyncOp)
	{
		// Create the window
		D3D9RenderWindow* renderWindow = new D3D9RenderWindow(mRenderSystem->getInstanceHandle());

		renderWindow->initialize(desc);

		D3D9RenderWindowPtr winPtr(renderWindow);
		mRenderSystem->registerRenderWindow(winPtr);

		asyncOp.completeOperation(std::static_pointer_cast<RenderWindow>(winPtr));
	}
}