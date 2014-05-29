#pragma once

#include "BsGLPrerequisites.h"
#include "BsRenderWindowManager.h"

namespace BansheeEngine
{
	class BS_RSGL_EXPORT GLRenderWindowManager : public RenderWindowManager
	{
	public:
		GLRenderWindowManager(GLRenderSystem* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowManager::createImpl()
		 */
		RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow);

	private:
		GLRenderSystem* mRenderSystem;
	};
}