#pragma once

#include "CmPrerequisites.h"
#include "CmCoreThreadAccessor.h"
#include "CmRenderWindow.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Structure containing parameters for starting the application.
	 */
	struct START_UP_DESC
	{
		String renderSystem; /**< Name of the render system plugin to use. */
		String renderer; /**< Name of the renderer plugin to use. */

		String input; /**< Name of the input plugin to use. */
		String sceneManager; /**< Name of the scene manager plugin to use. */

		RENDER_WINDOW_DESC primaryWindowDesc; /**< Describes the window to create during start-up. */

		Vector<String> importers; /**< A list of importer plugins to load. */
	};

	/**
	 * @brief	Represents the primary entry point to the engine. Handles
	 *			start-up, shutdown, primary loop and allows you to load and unload
	 *			plugins.
	 *
	 * @note	Sim thread only.
	 */
	class CM_EXPORT Application
	{
		public:
			Application();

			/**
			 * @brief	Starts the application using the specified options. 
			 * 			This is how you start the engine. Must be called before any other engine method.
			 */
			void startUp(START_UP_DESC& desc);

			/**
			 * @brief	Executes the main loop. This will update your components and modules, queue objects 
			 *			for rendering and run the simulation. Usually called immediately after startUp().
			 * 			
			 *			This will run infinitely until stopMainLoop is called (usually from another thread or internally).
			 */
			void runMainLoop();

			/**
			 * @brief	Stops a (infinite) main loop from running. The loop will complete its current cycle before stopping.
			 */
			void stopMainLoop();

			/**
			 * @brief	Frees up all resources allocated during startUp, and while the application was running.
			 */
			void shutDown();

			/**
			 * @brief	
			 */
			RenderWindowPtr getPrimaryWindow() const { return mPrimaryWindow; }

			/**
			 * @brief	Loads a plugin.
			 *
			 * @param	pluginName		Name of the plugin to load, without extension.
			 * @param	[out] library	Specify as not null to receive a reference to 
			 *							the loaded library.
			 * 
			 * @returns	Value returned from the plugin start-up method.
			 */
			void* loadPlugin(const String& pluginName, DynLib** library = nullptr);

			/**
			 * @brief	Unloads a previously loaded plugin. 
			 */
			void unloadPlugin(DynLib* library);

			/**
			 * @brief	Called every frame by the main loop, after scene update and before rendering.
			 */
			Event<void()> mainLoopCallback;

	private:
		/**
		 * @brief	Called when the frame finishes rendering.
		 */
		void frameRenderingFinishedCallback();

		/**
		 * @brief	Called by the core thread to begin profiling.
		 */
		void beginCoreProfiling();

		/**
		 * @brief	Called by the core thread to end profiling.
		 */
		void endCoreProfiling();

	private:
		RenderWindowPtr mPrimaryWindow;

		DynLib* mSceneManagerPlugin;

		bool mIsFrameRenderingFinished;
		CM_MUTEX(mFrameRenderingFinishedMutex);
		CM_THREAD_SYNCHRONISER(mFrameRenderingFinishedCondition);

		volatile bool mRunMainLoop;
	};

	/**
	 * @brief	Provides easy access to primary entry point for the engine.
	 */
	CM_EXPORT Application& gApplication();
}