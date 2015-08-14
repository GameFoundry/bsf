#include "BsD3D9RenderWindowManager.h"
#include "BsD3D9RenderAPI.h"
#include "BsD3D9RenderWindow.h"
#include "BsAsyncOp.h"

namespace BansheeEngine
{
	D3D9RenderWindowManager::D3D9RenderWindowManager(D3D9RenderAPI* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	RenderWindowPtr D3D9RenderWindowManager::createImpl(RENDER_WINDOW_DESC& desc, UINT32 windowId, const RenderWindowPtr& parentWindow)
	{
		if(parentWindow != nullptr)
		{
			UINT64 hWnd;
			parentWindow->getCustomAttribute("WINDOW", &hWnd);
			desc.platformSpecific["parentWindowHandle"] = toString(hWnd);
		}

		D3D9RenderWindow* window = new (bs_alloc<D3D9RenderWindow>()) D3D9RenderWindow(desc, windowId, mRenderSystem->getInstanceHandle());

		return RenderWindowPtr(window, &CoreObject::_delete<D3D9RenderWindow, GenAlloc>);
	}

	D3D9RenderWindowCoreManager::D3D9RenderWindowCoreManager(D3D9RenderAPI* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	SPtr<RenderWindowCore> D3D9RenderWindowCoreManager::createInternal(RENDER_WINDOW_DESC& desc, UINT32 windowId)
	{
		D3D9RenderWindowCore* window = new (bs_alloc<D3D9RenderWindowCore>()) D3D9RenderWindowCore(desc, windowId, mRenderSystem->getInstanceHandle());

		SPtr<D3D9RenderWindowCore> renderWindowPtr = bs_shared_ptr<D3D9RenderWindowCore>(window);
		renderWindowPtr->_setThisPtr(renderWindowPtr);

		windowCreated(window);

		return renderWindowPtr;
	}
}