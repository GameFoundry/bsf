#pragma once

#include "CmPrerequisites.h"
#include "CmHighLevelGpuProgram.h"
#include "CmRenderWindow.h"

namespace CamelotFramework
{
	class RenderWindow;
	class Viewport;
	class HighLevelGpuProgramManager;
}

namespace CamelotFramework
{
	struct START_UP_DESC
	{
		String renderSystem;
		String renderer;

		String input;
		String sceneManager;

		RENDER_WINDOW_DESC primaryWindowDesc;

		std::vector<String> importers;

		String resourceCacheDirectory;
	};

	class CM_EXPORT Application
	{
		public:
			Application();

			/**
			 * @brief	Starts the application using the specified options. 
			 * 			This is how you start the engine.
			 */
			void startUp(START_UP_DESC& desc);

			/**
			 * @brief	Executes the main loop. This will cause actually rendering to be performed
			 * 			and simulation to be run. Usually called immediately after startUp().
			 * 			
			 *			This will run infinitely until stopMainLoop is called (usually from another thread or internally).
			 */
			void runMainLoop();

			/**
			 * @brief	Stops a (infinite) main loop from running. The loop will complete its current cycle before stopping.
			 * 			You may call this from other threads.
			 */
			void stopMainLoop();

			/**
			 * @brief	Frees up all resources allocated during startUp, and while the application was running.
			 */
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
			void* loadPlugin(const String& pluginName);

	private:
		RenderWindowPtr mPrimaryRenderWindow;
		DeferredRenderContextPtr mPrimaryRenderContext;

		bool mIsFrameRenderingFinished;
		CM_MUTEX(mFrameRenderingFinishedMutex);

		volatile bool mRunMainLoop;

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