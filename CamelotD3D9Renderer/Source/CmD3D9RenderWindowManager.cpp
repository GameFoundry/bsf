#include "CmD3D9RenderWindowManager.h"
#include "CmD3D9RenderSystem.h"
#include "CmD3D9RenderWindow.h"
#include "CmAsyncOp.h"

namespace CamelotFramework
{
	D3D9RenderWindowManager::D3D9RenderWindowManager(D3D9RenderSystem* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	RenderWindowPtr D3D9RenderWindowManager::createImpl(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		if(parentWindow != nullptr)
		{
			HWND hWnd;
			parentWindow->getCustomAttribute("WINDOW", &hWnd);
			desc.platformSpecific["parentWindowHandle"] = toString((UINT64)hWnd);
		}

		D3D9RenderWindow* window = new (cm_alloc<D3D9RenderWindow, PoolAlloc>()) D3D9RenderWindow(desc, mRenderSystem->getInstanceHandle());

		return RenderWindowPtr(window, &CoreObject::_deleteDelayed<D3D9RenderWindow, PoolAlloc>);
	}
}