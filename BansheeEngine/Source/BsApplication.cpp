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

namespace BansheeEngine
{
	START_UP_DESC createStartUpDesc(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer)
	{
		START_UP_DESC desc;
		desc.renderSystem = renderSystem;
		desc.renderer = renderer;
		desc.primaryWindowDesc = primaryWindowDesc;

		desc.input = "BansheeOISInput";
		desc.importers.push_back("BansheeFreeImgImporter");
		desc.importers.push_back("BansheeFBXImporter");
		desc.importers.push_back("BansheeFontImporter");

		return desc;
	}

	Application::Application(RENDER_WINDOW_DESC& primaryWindowDesc, RenderSystemPlugin renderSystem, RendererPlugin renderer)
		:CoreApplication(createStartUpDesc(primaryWindowDesc, getLibNameForRenderSystem(renderSystem), getLibNameForRenderer(renderer))),
		mMonoPlugin(nullptr), mSBansheeEnginePlugin(nullptr)
	{
		PlainTextImporter* importer = bs_new<PlainTextImporter>();
		Importer::instance()._registerAssetImporter(importer);

		VirtualInput::startUp();
		ScriptManager::startUp();
		BuiltinResources::startUp(renderSystem);
		GUIManager::startUp();
		GUIMaterialManager::startUp();
		OverlayManager::startUp();

		Cursor::startUp();
	}

	Application::~Application()
	{
		// Need to clear all objects before I unload any plugins, as they
		// could have allocated parts or all of those objects.
		SceneManager::instance().clearScene(); 

#if BS_VER == BS_VER_DEV
		shutdownPlugin(mSBansheeEnginePlugin);
		unloadPlugin(mSBansheeEnginePlugin);

		shutdownPlugin(mMonoPlugin);
		unloadPlugin(mMonoPlugin);
#endif

		// Cleanup any new objects queued for destruction by unloaded scripts
		gCoreThread().update();
		gCoreThread().submitAccessors(true);

		Cursor::shutDown();

		GUIMaterialManager::instance().clearMaterials();

		OverlayManager::shutDown();
		GUIManager::shutDown();
		GUIMaterialManager::shutDown();
		BuiltinResources::shutDown();
		ScriptManager::shutDown();
		VirtualInput::shutDown();
	}

	void Application::onStartUp()
	{
#if BS_VER == BS_VER_DEV
		loadPlugin("BansheeMono", &mMonoPlugin);
		loadPlugin("SBansheeEngine", &mSBansheeEnginePlugin); // Scripting interface
#endif

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