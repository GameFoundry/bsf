#include "BsApplication.h"
#include "BsGUIMaterialManager.h"
#include "BsGUIManager.h"
#include "BsOverlayManager.h"
#include "BsShapeMeshes2D.h"
#include "BsShapeMeshes3D.h"
#include "BsBuiltinResources.h"
#include "BsScriptManager.h"
#include "BsProfilingManager.h"
#include "BsVirtualInput.h"
#include "BsCursor.h"

namespace BansheeEngine
{
	START_UP_DESC createStartUpDesc(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer)
	{
		START_UP_DESC desc;
		desc.renderSystem = renderSystem;
		desc.renderer = renderer;
		desc.primaryWindowDesc = primaryWindowDesc;

		desc.input = "BansheeOISInput";
		desc.sceneManager = "BansheeSceneManager";
		desc.importers.push_back("BansheeFreeImgImporter");
		desc.importers.push_back("BansheeFBXImporter");
		desc.importers.push_back("BansheeFontImporter");

		return desc;
	}

	Application::Application(RENDER_WINDOW_DESC& primaryWindowDesc, RenderSystemPlugin renderSystem, RendererPlugin renderer)
		:CoreApplication(createStartUpDesc(primaryWindowDesc, getLibNameForRenderSystem(renderSystem), getLibNameForRenderer(renderer))),
		mMonoPlugin(nullptr), mSBansheeEnginePlugin(nullptr)
	{
		VirtualInput::startUp();
		ScriptManager::startUp();
		BuiltinResources::startUp(renderSystem);
		GUIManager::startUp();
		GUIMaterialManager::startUp();
		OverlayManager::startUp();

		Cursor::startUp();

#if BS_VER == BS_VER_DEV
		loadPlugin("BansheeMono", &mMonoPlugin);
		loadPlugin("SBansheeEngine", &mSBansheeEnginePlugin); // Scripting interface
#endif
	}

	Application::~Application()
	{
#if BS_VER == BS_VER_DEV
		unloadPlugin(mSBansheeEnginePlugin);
		unloadPlugin(mMonoPlugin);
#endif

		Cursor::shutDown();

		GUIMaterialManager::instance().forceReleaseAllMaterials();

		OverlayManager::shutDown();
		GUIManager::shutDown();
		GUIMaterialManager::shutDown();
		BuiltinResources::shutDown();
		ScriptManager::shutDown();
		VirtualInput::shutDown();
	}

	void Application::onStartUp()
	{
		CoreApplication::onStartUp();

		Cursor::instance().setCursor(CursorType::Arrow);
	}

	void Application::startUp(RENDER_WINDOW_DESC& primaryWindowDesc, RenderSystemPlugin renderSystem, RendererPlugin renderer)
	{
		CoreApplication::startUp<Application>(primaryWindowDesc, renderSystem, renderer);
	}

	void Application::update()
	{
		CoreApplication::update();

		VirtualInput::instance()._update();
		PROFILE_CALL(GUIManager::instance().update(), "GUI");
	}

	const ViewportPtr& Application::getPrimaryViewport() const
	{
		// TODO - Need a way to determine primary viewport!
		return nullptr;
	}

	const String& Application::getLibNameForRenderSystem(RenderSystemPlugin plugin)
	{
		static String DX11Name = "BansheeD3D11RenderSystem";
		static String DX9Name = "BansheeD3D9RenderSystem";
		static String OpenGLName = "BansheeGLRenderSystem";

		switch (plugin)
		{
		case RenderSystemPlugin::DX11:
			return DX11Name;
		case RenderSystemPlugin::DX9:
			return DX9Name;
		case RenderSystemPlugin::OpenGL:
			return OpenGLName;
		}

		return StringUtil::BLANK;
	}

	const String& Application::getLibNameForRenderer(RendererPlugin plugin)
	{
		static String DefaultName = "BansheeRenderer";

		switch (plugin)
		{
		case RendererPlugin::Default:
			return DefaultName;
		}
	
		return StringUtil::BLANK;
	}

	Application& gApplication()
	{
		return static_cast<Application&>(Application::instance());
	}
}