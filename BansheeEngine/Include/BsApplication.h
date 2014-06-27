#pragma once

#include "BsPrerequisites.h"
#include "BsCoreApplication.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Types of available render systems.
	 */
	enum class RenderSystemPlugin
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
		Application(RENDER_WINDOW_DESC& primaryWindowDesc, RenderSystemPlugin renderSystem, RendererPlugin renderer);
		virtual ~Application();

		/**
		 * @brief	Starts the Banshee engine.
		 * 
		 * @param	primaryWindowDesc	Description of the primary render window that will be created on startup.
		 * @param	renderSystem		Render system to use.
		 * @param	renderer			Renderer to use.
		 */
		static void startUp(RENDER_WINDOW_DESC& primaryWindowDesc, RenderSystemPlugin renderSystem, RendererPlugin renderer);

		/**
		 * @brief	Returns the primary viewport of the application.
		 *
		 * @note	e.g. player or game view.
		 */
		const ViewportPtr& getPrimaryViewport() const;
	protected:
		/**
		 * @copydoc	Module::onStartUp
		 */
		virtual void onStartUp();

		/**
		 * @copydoc	CoreApplication::update.
		 */
		virtual void update();

	private:
		/**
		 * @brief	Translates render system type into library name.
		 */
		static const String& getLibNameForRenderSystem(RenderSystemPlugin plugin);

		/**
		 * @brief	Translates renderer type into library name.
		 */
		static const String& getLibNameForRenderer(RendererPlugin plugin);

		DynLib* mMonoPlugin;
		DynLib* mSBansheeEnginePlugin;
	};

	/**
	 * @copydoc	Application
	 */
	BS_EXPORT Application& gApplication();
}