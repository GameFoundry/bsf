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
			void shutDown();

			void DBG_renderSimpleFrame();

		private:
			HighLevelGpuProgramManager* mGpuProgramManager;

			RenderWindow* mRenderWindow;
			Viewport* mViewport;
			Camera* mCamera;
			HighLevelGpuProgramPtr mFragProg;
			HighLevelGpuProgramPtr mVertProg;

			DynLibManager* mDynLibManager; // TODO - Handle singletons differently
	};

	CM_EXPORT Application& gApplication();
}