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

	RenderWindowPtr D3D9RenderWindowManager::createImpl(const RENDER_WINDOW_DESC& desc)
	{
		return RenderWindowPtr(new D3D9RenderWindow(desc, mRenderSystem->getInstanceHandle()));
	}
}