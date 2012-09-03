#pragma once

#include <memory>

#include "CmRenderSystemFactory.h"

namespace Ogre
{
	class RenderSystem;
	typedef std::shared_ptr<Ogre::RenderSystem> RenderSystemPtr;
}

namespace CamelotEngine
{
	class Application
	{
		public:
			Application()
			{
				mActiveRenderSystem = RenderSystemManager::create("D3D9RenderSystem");
			}

			RenderSystemPtr getRenderSystem() { return mActiveRenderSystem; }

		private:
			RenderSystemPtr mActiveRenderSystem;
	};

	Application& gApplication();
}