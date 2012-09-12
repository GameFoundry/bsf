#pragma once

#include <memory>
#include "CmHighLevelGpuProgram.h"

namespace CamelotEngine
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
			HighLevelGpuProgramManager* mGpuProgramManager;

			RenderWindow* mRenderWindow;
			Viewport* mViewport;
			Camera* mCamera;
			HighLevelGpuProgramPtr mFragProg;
			HighLevelGpuProgramPtr mVertProg;
	};

	Application& gApplication();
}