#pragma once

#include "CmGLPrerequisites.h"
#include "CmRenderWindowManager.h"

namespace CamelotEngine
{
	class CM_RSGL_EXPORT GLRenderWindowManager : public RenderWindowManager
	{
	public:
		GLRenderWindowManager(GLRenderSystem* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowManager::create()
		 */
		void createImpl(const RENDER_WINDOW_DESC& desc, AsyncOp& asyncOp);

	private:
		GLRenderSystem* mRenderSystem;
	};
}