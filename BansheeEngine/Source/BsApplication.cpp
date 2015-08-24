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
#include "BsSceneManager.h"
#include "BsSceneObject.h"
#include "BsCursor.h"
#include "BsCoreThread.h"
#include "BsFileSystem.h"
#include "BsPlainTextImporter.h"
#include "BsImporter.h"
#include "BsShortcutManager.h"

namespace BansheeEngine
{
	START_UP_DESC createStartUpDesc(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderAPI, const String& renderer)
	{
		START_UP_DESC desc;
		desc.renderAPI = renderAPI;
		desc.renderer = renderer;
		desc.primaryWindowDesc = primaryWindowDesc;

		desc.input = "BansheeOISInput";
		desc.importers.push_back("BansheeFreeImgImporter");
		desc.importers.push_back("BansheeFBXImporter");
		desc.importers.push_back("BansheeFontImporter");
		desc.importers.push_back("BansheeSL");

		return desc;
	}

	Application::Application(RENDER_WINDOW_DESC& primaryWindowDesc, RenderAPIPlugin renderAPI, RendererPlugin renderer)
		:CoreApplication(createStartUpDesc(primaryWindowDesc, getLibNameForRenderAPI(renderAPI), getLibNameForRenderer(renderer))),
		mMonoPlugin(nullptr), mSBansheeEnginePlugin(nullptr)
	{

	}

	Application::~Application()
	{
		// Cleanup any new objects queued for destruction by unloaded scripts
		gCoreThread().update();
		gCoreThread().submitAccessors(true);

		Cursor::shutDown();

		GUIMaterialManager::instance().clearMaterials();

		ShortcutManager::shutDown();
		OverlayManager::shutDown();
		GUIManager::shutDown();
		GUIMaterialManager::shutDown();
		BuiltinResources::shutDown();
		VirtualInput::shutDown();
	}

	void Application::onStartUp()
	{
		CoreApplication::onStartUp();

		PlainTextImporter* importer = bs_new<PlainTextImporter>();
		Importer::instance()._registerAssetImporter(importer);

		VirtualInput::startUp();
		BuiltinResources::startUp();
		GUIManager::startUp();
		GUIMaterialManager::startUp();
		OverlayManager::startUp();
		ShortcutManager::startUp();

		Cursor::startUp();
		Cursor::instance().setCursor(CursorType::Arrow);

		loadScriptSystem();
		ScriptManager::instance().initialize();
	}

	void Application::onShutDown()
	{
		// Need to clear all objects before I unload any plugins, as they
		// could have allocated parts or all of those objects.
		SceneManager::instance().clearScene(true);

		unloadScriptSystem();

		CoreApplication::onShutDown();
	}

	void Application::startUp(RENDER_WINDOW_DESC& primaryWindowDesc, RenderAPIPlugin renderAPI, RendererPlugin renderer)
	{
		CoreApplication::startUp<Application>(primaryWindowDesc, renderAPI, renderer);
	}

	void Application::preUpdate()
	{
		CoreApplication::preUpdate();

		VirtualInput::instance()._update();
	}

	void Application::postUpdate()
	{
		CoreApplication::postUpdate();

		PROFILE_CALL(GUIManager::instance().update(), "GUI");
	}

	void Application::loadScriptSystem()
	{
		ScriptManager::startUp();

		loadPlugin("BansheeMono", &mMonoPlugin);
		loadPlugin("SBansheeEngine", &mSBansheeEnginePlugin); 
	}

	void Application::unloadScriptSystem()
	{
		ScriptManager::shutDown();

		// These plugins must be unloaded before any other script plugins, because
		// they will cause finalizers to trigger and various modules those finalizers
		// might reference must still be active
		unloadPlugin(mSBansheeEnginePlugin);
		unloadPlugin(mMonoPlugin);
	}

	ViewportPtr Application::getPrimaryViewport() const
	{
		// TODO - Need a way to determine primary viewport!
		return nullptr;
	}

	Path Application::getEngineAssemblyPath() const
	{
		Path assemblyPath = getBuiltinAssemblyFolder();
		assemblyPath.append(toWString(String(ENGINE_ASSEMBLY)) + L".dll");
		
		return assemblyPath;
	}

	Path Application::getGameAssemblyPath() const
	{
		Path assemblyPath = getScriptAssemblyFolder();
		assemblyPath.append(toWString(String(SCRIPT_GAME_ASSEMBLY)) + L".dll");

		return assemblyPath;
	}

	Path Application::getBuiltinAssemblyFolder() const
	{
		Path assemblyFolder = FileSystem::getWorkingDirectoryPath();
		assemblyFolder.append(ASSEMBLY_PATH);

		return assemblyFolder;
	}

	Path Application::getScriptAssemblyFolder() const
	{
		Path assemblyFolder = FileSystem::getWorkingDirectoryPath();
		assemblyFolder.append(ASSEMBLY_PATH);

		return assemblyFolder;
	}

	const String& Application::getLibNameForRenderAPI(RenderAPIPlugin plugin)
	{
		static String DX11Name = "BansheeD3D11RenderAPI";
		static String DX9Name = "BansheeD3D9RenderAPI";
		static String OpenGLName = "BansheeGLRenderAPI";

		switch (plugin)
		{
		case RenderAPIPlugin::DX11:
			return DX11Name;
		case RenderAPIPlugin::DX9:
			return DX9Name;
		case RenderAPIPlugin::OpenGL:
			return OpenGLName;
		}

		return StringUtil::BLANK;
	}

	const String& Application::getLibNameForRenderer(RendererPlugin plugin)
	{
		static String DefaultName = "RenderBeast";

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