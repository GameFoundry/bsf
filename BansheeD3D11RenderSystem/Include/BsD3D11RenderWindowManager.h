#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsRenderWindowManager.h"

namespace BansheeEngine
{
	/**
	 * @copydoc	RenderWindowManager
	 */
	class BS_D3D11_EXPORT D3D11RenderWindowManager : public RenderWindowManager
	{
	public:
		D3D11RenderWindowManager(D3D11RenderAPI* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowManager::createImpl
		 */
		RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, const RenderWindowPtr& parentWindow);

	private:
		D3D11RenderAPI* mRenderSystem;
	};

	/**
	 * @copydoc	RenderWindowCoreManager
	 */
	class BS_D3D11_EXPORT D3D11RenderWindowCoreManager : public RenderWindowCoreManager
	{
	public:
		D3D11RenderWindowCoreManager(D3D11RenderAPI* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowCoreManager::createInternal
		 */
		virtual SPtr<RenderWindowCore> createInternal(RENDER_WINDOW_DESC& desc);

	private:
		D3D11RenderAPI* mRenderSystem;
	};
}