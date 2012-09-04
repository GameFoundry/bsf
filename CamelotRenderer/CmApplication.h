#pragma once

#include <memory>

namespace Ogre
{
	class RenderWindow;
	class Viewport;
}

namespace CamelotEngine
{
	class Application
	{
		public:
			Application();

			void startUp();
			void shutDown();

			void DBG_renderSimpleFrame();

		private:
			Ogre::RenderWindow* mRenderWindow;
			Ogre::Viewport* mViewport;
	};

	Application& gApplication();
}