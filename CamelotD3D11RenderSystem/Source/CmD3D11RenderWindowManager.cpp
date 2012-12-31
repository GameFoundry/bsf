#include "CmD3D11RenderWindowManager.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11RenderWindow.h"
#include "CmAsyncOp.h"

namespace CamelotEngine
{
	D3D11RenderWindowManager::D3D11RenderWindowManager(D3D11RenderSystem* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	void D3D11RenderWindowManager::createImpl(const RENDER_WINDOW_DESC& desc, AsyncOp& asyncOp)
	{
		RenderSystem* rs = RenderSystem::instancePtr();
		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(rs);

		// Create the window
		D3D11RenderWindow* renderWindow = new D3D11RenderWindow(d3d11rs->getPrimaryDevice(), d3d11rs->getDXGIFactory());
		renderWindow->initialize(desc);

		D3D11RenderWindowPtr winPtr(renderWindow);
		asyncOp.completeOperation(std::static_pointer_cast<RenderWindow>(winPtr));
	}
}