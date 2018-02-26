//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "Managers/BsRenderWindowManager.h"

namespace bs
{
	/** @addtogroup D3D11
	 *  @{
	 */

	/** @copydoc RenderWindowManager */
	class D3D11RenderWindowManager : public RenderWindowManager
	{
	public:
		D3D11RenderWindowManager(ct::D3D11RenderAPI* renderSystem);

	protected:
		/** @copydoc RenderWindowManager::createImpl */
		SPtr<RenderWindow> createImpl(RENDER_WINDOW_DESC& desc, UINT32 windowId, 
			const SPtr<RenderWindow>& parentWindow) override;

	private:
		ct::D3D11RenderAPI* mRenderSystem;
	};

	/** @} */
}