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

			void startUp(const String& renderSystemDll, const String& rendererDll);
			void runMainLoop();
			void shutDown();

			UINT32 getAppWindowId();

			RenderWindow* getPrimaryRenderWindow() const { return mPrimaryRenderWindow; }

			/**
			 * @brief	Loads a plugin.
			 *
			 * @param	pluginName	Name of the plugin to load, without extension.
			 */
			void loadPlugin(const String& pluginName);

	private:
		RenderWindow* mPrimaryRenderWindow;

		/**
		 * @brief	Callback called from the render thread in order to initialize resources.
		 */
		void updateResourcesCallback();

		/**
		 * @brief	Runs the OS specific message pump.
		 */
		void updateMessagePump();
	};

	CM_EXPORT Application& gApplication();
}