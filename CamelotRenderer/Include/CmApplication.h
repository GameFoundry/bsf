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

			UINT64 getAppWindowId();

			RenderWindowPtr getPrimaryRenderWindow() const { return mPrimaryRenderWindow; }

			// TODO: This is just for debug purposes. Normally I'd want to have one render context per scene view, not one global one
			DeferredRenderContextPtr getPrimaryRenderContext() const { return mPrimaryRenderContext; }

			/**
			 * @brief	Loads a plugin.
			 *
			 * @param	pluginName	Name of the plugin to load, without extension.
			 */
			void loadPlugin(const String& pluginName);

	private:
		RenderWindowPtr mPrimaryRenderWindow;
		DeferredRenderContextPtr mPrimaryRenderContext;

		bool mIsFrameRenderingFinished;
		CM_MUTEX(mFrameRenderingFinishedMutex);

		/**
		 * @brief	Callback called from the render thread in order to initialize resources.
		 */
		void updateResourcesCallback();

		/**
		 * @brief	Runs the OS specific message pump.
		 */
		void updateMessagePump();

		/**
		 * @brief	Called when the frame finishes rendering.
		 */
		void frameRenderingFinishedCallback();
	};

	CM_EXPORT Application& gApplication();
}