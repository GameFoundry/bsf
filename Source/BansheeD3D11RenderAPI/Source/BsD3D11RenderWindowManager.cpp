//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD3D11RenderWindowManager.h"
#include "BsD3D11RenderAPI.h"
#include "BsD3D11RenderWindow.h"
#include "BsAsyncOp.h"

namespace bs
{
	D3D11RenderWindowManager::D3D11RenderWindowManager(ct::D3D11RenderAPI* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	SPtr<RenderWindow> D3D11RenderWindowManager::createImpl(RENDER_WINDOW_DESC& desc, UINT32 windowId, 
		const SPtr<RenderWindow>& parentWindow)
	{
		ct::RenderAPI* rs = ct::RenderAPI::instancePtr();
		ct::D3D11RenderAPI* d3d11rs = static_cast<ct::D3D11RenderAPI*>(rs);

		if(parentWindow != nullptr)
		{
			UINT64 hWnd;
			parentWindow->getCustomAttribute("WINDOW", &hWnd);
			desc.platformSpecific["parentWindowHandle"] = toString(hWnd);
		}

		// Create the window
		D3D11RenderWindow* renderWindow = new (bs_alloc<D3D11RenderWindow>()) D3D11RenderWindow(desc, windowId, 
			d3d11rs->getPrimaryDevice(), d3d11rs->getDXGIFactory());
		return bs_core_ptr<D3D11RenderWindow>(renderWindow);
	}

	namespace ct
	{
	D3D11RenderWindowManager::D3D11RenderWindowManager(D3D11RenderAPI* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	SPtr<RenderWindow> D3D11RenderWindowManager::createInternal(RENDER_WINDOW_DESC& desc, UINT32 windowId)
	{
		RenderAPI* rs = RenderAPI::instancePtr();
		D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(rs);

		// Create the window
		D3D11RenderWindow* renderWindow = new (bs_alloc<D3D11RenderWindow>()) D3D11RenderWindow(desc, windowId, 
			d3d11rs->getPrimaryDevice(), d3d11rs->getDXGIFactory());

		SPtr<D3D11RenderWindow> renderWindowPtr = bs_shared_ptr<D3D11RenderWindow>(renderWindow);
		renderWindowPtr->_setThisPtr(renderWindowPtr);

		windowCreated(renderWindow);

		return renderWindowPtr;
	}
	}
}