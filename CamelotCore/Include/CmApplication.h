#pragma once

#include "CmPrerequisites.h"
#include "CmCoreThreadAccessor.h"
#include "CmHighLevelGpuProgram.h"
#include "CmRenderWindow.h"

#include "boost/signal.hpp"

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

		Vector<String>::type importers;
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

			RenderWindowPtr getPrimaryWindow() const { return mPrimaryWindow; }

			/**
			 * @brief	Loads a plugin.
			 *
			 * @param	pluginName	Name of the plugin to load, without extension.
			 */
			void* loadPlugin(const String& pluginName);

			/**
			 * @brief	Called every frame by the main loop, after scene update and before rendering.
			 */
			boost::signal<void()> mainLoopCallback;

	private:
		friend CM_EXPORT CoreAccessor& gMainCA();
		friend CM_EXPORT SyncedCoreAccessor& gMainSyncedCA();

		RenderWindowPtr mPrimaryWindow;
		CoreAccessorPtr mPrimaryCoreAccessor;
		SyncedCoreAccessor* mPrimarySyncedCoreAccessor;

		bool mIsFrameRenderingFinished;
		CM_MUTEX(mFrameRenderingFinishedMutex);
		CM_THREAD_SYNCHRONISER(mFrameRenderingFinishedCondition);

		volatile bool mRunMainLoop;

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
	};

	CM_EXPORT Application& gApplication();

	/**
	 * @brief	A shortcut for accessing the primary core accessor. It may only be accessed safely
	 * 			from the main thread.
	 */
	CM_EXPORT CoreAccessor& gMainCA();

	/**
	 * @brief	A shortcut for accessing the primary synchronized core accessor. This accessor may be accessed
	 * 			from all threads except the core thread. All operations from this accessor will be executed after
	 * 			non-synchronized primary accessor has finished executing.
	 * 			
	 * @note	It is more efficient to create your own non-synchronized core accessor if you plan on often using it from
	 * 			threads other than main.
	 */
	CM_EXPORT SyncedCoreAccessor& gMainSyncedCA();
}