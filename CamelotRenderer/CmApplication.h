#pragma once

#include <memory>
#include "OgreHighLevelGpuProgram.h"

namespace Ogre
{
	class RenderWindow;
	class Viewport;
	class Camera;
	class HighLevelGpuProgramManager;
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
			Ogre::HighLevelGpuProgramManager* mGpuProgramManager;

			Ogre::RenderWindow* mRenderWindow;
			Ogre::Viewport* mViewport;
			Ogre::Camera* mCamera;
			Ogre::HighLevelGpuProgramPtr mFragProg;
			Ogre::HighLevelGpuProgramPtr mVertProg;
	};

	Application& gApplication();
}