#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmRenderWindowManager.h"

namespace CamelotEngine
{
	class CM_D3D9_EXPORT D3D9RenderWindowManager : public RenderWindowManager
	{
	public:
		D3D9RenderWindowManager(D3D9RenderSystem* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowManager::createImpl()
		 */
		RenderWindow* createImpl(const RENDER_WINDOW_DESC& desc);

	private:
		D3D9RenderSystem* mRenderSystem;
	};
}