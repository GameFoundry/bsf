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

			UINT32 getAppWindowId();

			/**
			 * @brief	Loads a plugin.
			 *
			 * @param	pluginName	Name of the plugin to load, without extension.
			 */
			void loadPlugin(const String& pluginName);

		private:
			RenderWindow* mRenderWindow;
			std::shared_ptr<Camera> mCamera;
			HighLevelGpuProgramPtr mFragProg;
			HighLevelGpuProgramPtr mVertProg;
			TextureRef mDbgTexture;
			MeshRef mDbgMesh;
			GameObjectPtr mCameraGO;
	};

	CM_EXPORT Application& gApplication();
}