//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsApplication.h"
#include "BsCrashHandler.h"
#include "BsCoreThread.h"
#include "BsFileSerializer.h"
#include "BsGameSettings.h"
#include "BsFileSystem.h"
#include "BsResources.h"
#include "BsResourceManifest.h"
#include "BsPrefab.h"
#include "BsSceneObject.h"
#include "BsSceneManager.h"
#include "BsRenderAPI.h"
#include "BsGameResourceManager.h"

void runApplication();

#if BS_PLATFORM == BS_PLATFORM_WIN32
#include <windows.h>

using namespace BansheeEngine;

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	CrashHandler::startUp();

	__try
	{
		runApplication();
	}
	__except (gCrashHandler().reportCrash(GetExceptionInformation()))
	{
		PlatformUtility::terminate(true);
	}

	CrashHandler::shutDown();

	return 0;
}
#endif // End BS_PLATFORM

using namespace BansheeEngine;

void runApplication()
{
	Path gameSettingsPath = Paths::getGameSettingsPath();

	FileDecoder fd(gameSettingsPath);
	SPtr<GameSettings> gameSettings = std::static_pointer_cast<GameSettings>(fd.decode());

	if (gameSettings == nullptr)
		gameSettings = bs_shared_ptr_new<GameSettings>();

	unsigned int resolutionWidth = 200;
	unsigned int resolutionHeight = 200;

	if (!gameSettings->fullscreen)
	{
		resolutionWidth = gameSettings->resolutionWidth;
		resolutionHeight = gameSettings->resolutionHeight;
	}

	RENDER_WINDOW_DESC renderWindowDesc;
	renderWindowDesc.videoMode = VideoMode(resolutionWidth, resolutionHeight);
	renderWindowDesc.title = toString(gameSettings->titleBarText);
	renderWindowDesc.fullscreen = false;
	renderWindowDesc.hidden = gameSettings->fullscreen;

	Application::startUp(renderWindowDesc, RenderAPIPlugin::DX11);

	// Note: What if script tries to load resources during startup? The manifest nor the mapping wont be set up yet.
	Path resourcesPath = Paths::getGameResourcesPath();
	Path resourceMappingPath = resourcesPath + GAME_RESOURCE_MAPPING_NAME;

	FileDecoder mappingFd(resourceMappingPath);
	SPtr<ResourceMapping> resMapping = std::static_pointer_cast<ResourceMapping>(mappingFd.decode());

	GameResourceManager::instance().setMapping(resMapping);

	if (gameSettings->fullscreen)
	{
		if (gameSettings->useDesktopResolution)
		{
			const VideoModeInfo& videoModeInfo = RenderAPI::getVideoModeInfo();
			const VideoOutputInfo& primaryMonitorInfo = videoModeInfo.getOutputInfo(0);
			const VideoMode& selectedVideoMode = primaryMonitorInfo.getDesktopVideoMode();

			RenderWindowPtr window = gApplication().getPrimaryWindow();
			window->setFullscreen(gCoreAccessor(), selectedVideoMode);

			resolutionWidth = selectedVideoMode.getWidth();
			resolutionHeight = selectedVideoMode.getHeight();
		}
		else
		{
			resolutionWidth = gameSettings->resolutionWidth;
			resolutionHeight = gameSettings->resolutionHeight;

			VideoMode videoMode(resolutionWidth, resolutionHeight);

			RenderWindowPtr window = gApplication().getPrimaryWindow();
			window->show(gCoreAccessor());
			window->setFullscreen(gCoreAccessor(), videoMode);
		}
	}

	gameSettings->useDesktopResolution = false; // Not relevant after first startup

	// TODO - Save full video mode
	gameSettings->resolutionWidth = resolutionWidth;
	gameSettings->resolutionHeight = resolutionHeight;

	FileEncoder fe(gameSettingsPath);
	fe.encode(gameSettings.get());

	Path resourceManifestPath = resourcesPath + GAME_RESOURCE_MANIFEST_NAME;

	ResourceManifestPtr manifest;
	if (FileSystem::exists(resourceManifestPath))
	{
		Path resourceRoot = FileSystem::getWorkingDirectoryPath();
		resourceRoot.append(resourcesPath);
		resourceRoot.makeParent(); // Remove /Resources entry, as we expect all resources to be relative to that path

		manifest = ResourceManifest::load(resourceManifestPath, resourceRoot);

		gResources().registerResourceManifest(manifest);
	}

	{
		HPrefab mainScene = static_resource_cast<Prefab>(gResources().loadFromUUID(gameSettings->mainSceneUUID, false, true, false));
		if (mainScene.isLoaded(false))
		{
			HSceneObject root = mainScene->instantiate();
			HSceneObject oldRoot = gSceneManager().getRootNode();

			gSceneManager()._setRootNode(root);
			oldRoot->destroy();
		}
	}

	Application::instance().runMainLoop();
	Application::shutDown();
}