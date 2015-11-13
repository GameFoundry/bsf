#pragma once

#include "BsPrerequisites.h"
#include "BsCoreApplication.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Types of available render systems.
	 */
	enum class RenderAPIPlugin
	{
		DX11,
		DX9,
		OpenGL
	};

	/**
	 * @brief	Types of available renderers.
	 */
	enum class RendererPlugin
	{
		Default
	};

	/**
	 * @brief	Primary entry point for Banshee engine. Handles startup and shutdown.
	 */
	class BS_EXPORT Application : public CoreApplication
	{
	public:
		Application(RENDER_WINDOW_DESC& primaryWindowDesc, RenderAPIPlugin renderAPI, RendererPlugin renderer);
		virtual ~Application();

		/**
		 * @brief	Starts the Banshee engine.
		 * 
		 * @param	primaryWindowDesc	Description of the primary render window that will be created on startup.
		 * @param	renderSystem		Render system to use.
		 * @param	renderer			Renderer to use.
		 */
		static void startUp(RENDER_WINDOW_DESC& primaryWindowDesc, RenderAPIPlugin renderAPI, RendererPlugin renderer = RendererPlugin::Default);

		/**
		 * @brief	Returns the primary viewport of the application.
		 *
		 * @note	e.g. player or game view.
		 */
		ViewportPtr getPrimaryViewport() const;

		/**
		 * @brief	Returns the absolute path to the builtin managed engine assembly file.
		 */
		Path getEngineAssemblyPath() const;

		/**
		 * @brief	Returns the absolute path to the game managed assembly file.
		 */
		Path getGameAssemblyPath() const;

		/**
		 * @brief	Returns the absolute path to the folder where built-in assemblies are located in.
		 */
		virtual Path getBuiltinAssemblyFolder() const;

		/**
		 * @brief	Returns the absolute path to the folder where script assemblies are located in.
		 */
		virtual Path getScriptAssemblyFolder() const;

	protected:
		/**
		 * @copydoc	Module::onStartUp
		 */
		virtual void onStartUp() override;

		/**
		 * @copydoc	Module::onShutDown
		 */
		virtual void onShutDown() override;

		/**
		 * @copydoc	CoreApplication::preUpdate.
		 */
		virtual void preUpdate() override;

		/**
		 * @copydoc	CoreApplication::postUpdate.
		 */
		virtual void postUpdate() override;

		/**
		 * @copydoc	CoreApplication::startUpRenderer.
		 */
		virtual void startUpRenderer() override;

		/**
		 * @brief	Loads the script system and all script libraries.
		 */
		virtual void loadScriptSystem();

		/**
		 * @brief	Unloads script libraries and shuts down the script system.
		 */
		virtual void unloadScriptSystem();

	protected:
		/**
		 * @brief	Translates render system type into library name.
		 */
		static String getLibNameForRenderAPI(RenderAPIPlugin plugin);

		/**
		 * @brief	Translates renderer type into library name.
		 */
		static String getLibNameForRenderer(RendererPlugin plugin);

		DynLib* mMonoPlugin;
		DynLib* mSBansheeEnginePlugin;
		String mRenderer;
	};

	/**
	 * @copydoc	Application
	 */
	BS_EXPORT Application& gApplication();
}