#pragma once

#include "BsGLPrerequisites.h"
#include "BsRenderWindowManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manager that handles window creation for OpenGL.
	 */
	class BS_RSGL_EXPORT GLRenderWindowManager : public RenderWindowManager
	{
	public:
		GLRenderWindowManager(GLRenderSystem* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowManager::createImpl()
		 */
		RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, const RenderWindowPtr& parentWindow);

	private:
		GLRenderSystem* mRenderSystem;
	};

	/**
	 * @brief	Manager that handles window creation for OpenGL.
	 */
	class BS_RSGL_EXPORT GLRenderWindowCoreManager : public RenderWindowCoreManager
	{
	public:
		GLRenderWindowCoreManager(GLRenderSystem* renderSystem);

	protected:
		/**
		 * @copydoc RenderWindowCoreManager::createInternal
		 */
		SPtr<RenderWindowCore> createInternal(RENDER_WINDOW_DESC& desc);

	private:
		GLRenderSystem* mRenderSystem;
	};
}