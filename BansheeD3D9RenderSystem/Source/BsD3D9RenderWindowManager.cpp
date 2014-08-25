#include "BsD3D9RenderWindowManager.h"
#include "BsD3D9RenderSystem.h"
#include "BsD3D9RenderWindow.h"
#include "BsAsyncOp.h"

namespace BansheeEngine
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

		D3D9RenderWindow* window = new (bs_alloc<D3D9RenderWindow, PoolAlloc>()) D3D9RenderWindow(mRenderSystem->getInstanceHandle());

		return RenderWindowPtr(window, &CoreObject::_deleteDelayed<D3D9RenderWindow, PoolAlloc>);
	}
}