//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
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
