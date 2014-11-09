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
		D3D9RenderWindowManager(D3D9RenderSystem* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowManager::createImpl()
		 */
		RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, const RenderWindowPtr& parentWindow);

	private:
		D3D9RenderSystem* mRenderSystem;
	};

	/**
	 * @brief	Handles creation of windows for DirectX 9.
	 */
	class BS_D3D9_EXPORT D3D9RenderWindowCoreManager : public RenderWindowCoreManager
	{
	public:
		D3D9RenderWindowCoreManager(D3D9RenderSystem* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowCoreManager::createInternal
		 */
		SPtr<RenderWindowCore> createInternal(RENDER_WINDOW_DESC& desc);

	private:
		D3D9RenderSystem* mRenderSystem;
	};
}