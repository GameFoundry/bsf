#pragma once

#include "CmPrerequisites.h"
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
	class CM_EXPORT Application
	{
		public:
			Application();

			void startUp(String renderSystemDll);
			void runMainLoop();
			void shutDown();

			void DBG_renderSimpleFrame();

		private:
			RenderWindow* mRenderWindow;
			Camera* mCamera;
			HighLevelGpuProgramPtr mFragProg;
			HighLevelGpuProgramPtr mVertProg;
			TexturePtr mDbgTexture;
	};

	CM_EXPORT Application& gApplication();
}