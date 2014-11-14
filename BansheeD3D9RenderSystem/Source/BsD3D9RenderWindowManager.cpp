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

	RenderWindowPtr D3D9RenderWindowManager::createImpl(RENDER_WINDOW_DESC& desc, const RenderWindowPtr& parentWindow)
	{
		if(parentWindow != nullptr)
		{
			HWND hWnd;
			parentWindow->getCustomAttribute("WINDOW", &hWnd);
			desc.platformSpecific["parentWindowHandle"] = toString((UINT64)hWnd);
		}

		D3D9RenderWindow* window = new (bs_alloc<D3D9RenderWindow, PoolAlloc>()) D3D9RenderWindow(desc, mRenderSystem->getInstanceHandle());

		return RenderWindowPtr(window, &CoreObject::_deleteDelayed<D3D9RenderWindow, PoolAlloc>);
	}

	D3D9RenderWindowCoreManager::D3D9RenderWindowCoreManager(D3D9RenderSystem* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	SPtr<RenderWindowCore> D3D9RenderWindowCoreManager::createInternal(RENDER_WINDOW_DESC& desc)
	{
		D3D9RenderWindowCore* window = new (bs_alloc<D3D9RenderWindowCore, GenAlloc>()) D3D9RenderWindowCore(desc, mRenderSystem->getInstanceHandle());

		SPtr<D3D9RenderWindowCore> renderWindowPtr = bs_shared_ptr<D3D9RenderWindowCore, GenAlloc>(window);
		renderWindowPtr->_setThisPtr(renderWindowPtr);

		windowCreated(window);

		return renderWindowPtr;
	}
}