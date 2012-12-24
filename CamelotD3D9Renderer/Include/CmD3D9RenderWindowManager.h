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
		 * @copydoc RenderWindowManager::create()
		 */
		void createImpl(const String& name, UINT32 width, UINT32 height, 
			bool fullScreen, const map<String, String>::type& miscParams, AsyncOp& asyncOp);

	private:
		D3D9RenderSystem* mRenderSystem;
	};
}