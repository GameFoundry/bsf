#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderWindowManager.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RenderWindowManager : public RenderWindowManager
	{
	public:
		D3D11RenderWindowManager(D3D11RenderSystem* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowManager::create()
		 */
		void createImpl(const RENDER_WINDOW_DESC& desc, AsyncOp& asyncOp);

	private:
		D3D11RenderSystem* mRenderSystem;
	};
}