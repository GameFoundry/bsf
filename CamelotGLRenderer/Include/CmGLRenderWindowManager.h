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
		void createImpl(const String& name, UINT32 width, UINT32 height, 
			bool fullScreen, const map<String, String>::type& miscParams, AsyncOp& asyncOp);

	private:
		GLRenderSystem* mRenderSystem;
	};
}