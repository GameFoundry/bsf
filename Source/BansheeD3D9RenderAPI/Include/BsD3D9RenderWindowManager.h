//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsRenderWindowManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of windows for DirectX 9.
	 */
	class BS_D3D9_EXPORT D3D9RenderWindowManager : public RenderWindowManager
	{
	public:
		D3D9RenderWindowManager(D3D9RenderAPI* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowManager::createImpl()
		 */
		RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, UINT32 windowId, const RenderWindowPtr& parentWindow) override;

	private:
		D3D9RenderAPI* mRenderSystem;
	};

	/**
	 * @brief	Handles creation of windows for DirectX 9.
	 */
	class BS_D3D9_EXPORT D3D9RenderWindowCoreManager : public RenderWindowCoreManager
	{
	public:
		D3D9RenderWindowCoreManager(D3D9RenderAPI* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowCoreManager::createInternal
		 */
		SPtr<RenderWindowCore> createInternal(RENDER_WINDOW_DESC& desc, UINT32 windowId) override;

	private:
		D3D9RenderAPI* mRenderSystem;
	};
}