#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsCoreThreadAccessor.h"
#include "BsRenderWindow.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Structure containing parameters for starting the application.
	 */
	struct START_UP_DESC
	{
		String renderAPI; /**< Name of the render system plugin to use. */
		String renderer; /**< Name of the renderer plugin to use. */

		String input; /**< Name of the input plugin to use. */

		RENDER_WINDOW_DESC primaryWindowDesc; /**< Describes the window to create during start-up. */

		Vector<String> importers; /**< A list of importer plugins to load. */
	};

	/**
	 * @brief	Represents the primary entry point for the core systems. Handles
	 *			start-up, shutdown, primary loop and allows you to load and unload
	 *			plugins.
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT CoreApplication : public Module<CoreApplication>
	{
		public:
			CoreApplication(START_UP_DESC& desc);
			virtual ~CoreApplication();

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
			 * @brief	Returns the main window that was created on application start-up.
			 */
			RenderWindowPtr getPrimaryWindow() const { return mPrimaryWindow; }

			/**
			 * @brief	Returns the id of the simulation thread.
			 *
			 * @note	Thread safe.
			 */
			BS_THREAD_ID_TYPE getSimThreadId() { return mSimThreadId; }

			/**
			 * @brief	Loads a plugin.
			 *
			 * @param	pluginName		Name of the plugin to load, without extension.
			 * @param	[out] library	Specify as not null to receive a reference to 
			 *							the loaded library.
			 * @param	passThrough		Optional parameter that will be passed to the loadPlugin function.
			 * 
			 * @returns	Value returned from the plugin start-up method.
			 */
			void* loadPlugin(const String& pluginName, DynLib** library = nullptr, void* passThrough = nullptr);

			/**
			 * @brief	Unloads a previously loaded plugin. 
			 */
			void unloadPlugin(DynLib* library);

			/**
			 * @brief	Calls the shutdown method on the plugin.
			 *
			 * @note	This is separate from "unload" method and should be called
			 *			before unload.
			 */
			void shutdownPlugin(DynLib* library);

	protected:
		/**
		 * @copydoc	Module::onStartUp
		 */
		virtual void onStartUp() override;

		/**
		 * @brief	Called for each iteration of the main loop. Called before any game objects or plugins are updated.
		 */
		virtual void preUpdate();

		/**
		 * @brief	Called for each iteration of the main loop. Called after all game objects and plugins are updated.
		 */
		virtual void postUpdate();

		/**
		 * @brief	Returns a handler that is used for resolving shader include file paths.
		 */
		virtual ShaderIncludeHandlerPtr getShaderIncludeHandler() const;

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
		typedef void(*UpdatePluginFunc)();

		RenderWindowPtr mPrimaryWindow;
		START_UP_DESC mStartUpDesc;

		DynLib* mRendererPlugin;

		Map<DynLib*, UpdatePluginFunc> mPluginUpdateFunctions;

		bool mIsFrameRenderingFinished;
		BS_MUTEX(mFrameRenderingFinishedMutex);
		BS_THREAD_SYNCHRONISER(mFrameRenderingFinishedCondition);
		BS_THREAD_ID_TYPE mSimThreadId;

		volatile bool mRunMainLoop;
	};

	/**
	 * @brief	Provides easy access to primary entry point for the engine.
	 */
	BS_CORE_EXPORT CoreApplication& gCoreApplication();
}