#pragma once

#include "BsPrerequisites.h"
#include "BsCoreApplication.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	enum class RenderSystemPlugin
	{
		DX11,
		DX9,
		OpenGL
	};

	enum class RendererPlugin
	{
		Default
	};

	class BS_EXPORT Application : public CoreApplication
	{
	public:
		Application(RENDER_WINDOW_DESC& primaryWindowDesc, RenderSystemPlugin renderSystem, RendererPlugin renderer);
		virtual ~Application();

		static void startUp(RENDER_WINDOW_DESC& primaryWindowDesc, RenderSystemPlugin renderSystem, RendererPlugin renderer);

		const ViewportPtr& getPrimaryViewport() const;
	protected:
		virtual void onStartUp();

		virtual void update();

	private:
		static const String& getLibNameForRenderSystem(RenderSystemPlugin plugin);
		static const String& getLibNameForRenderer(RendererPlugin plugin);

		DynLib* mMonoPlugin;
		DynLib* mSBansheeEnginePlugin;
	};

	BS_EXPORT Application& gApplication();
}