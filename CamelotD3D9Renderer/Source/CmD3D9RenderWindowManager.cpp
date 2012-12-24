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

	void D3D9RenderWindowManager::createImpl(const String& name, UINT32 width, UINT32 height, 
		bool fullScreen, const map<String, String>::type& miscParams, AsyncOp& asyncOp)
	{
		// Create the window
		D3D9RenderWindow* renderWindow = new D3D9RenderWindow(mRenderSystem->getInstanceHandle());

		renderWindow->initialize(name, width, height, fullScreen, &miscParams);

		D3D9RenderWindowPtr winPtr(renderWindow);
		mRenderSystem->registerRenderWindow(winPtr);

		asyncOp.completeOperation(std::static_pointer_cast<RenderWindow>(winPtr));
	}
}