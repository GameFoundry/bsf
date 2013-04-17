#include "BsApplication.h"
#include "BsGUIMaterialManager.h"
#include "BsGUIManager.h"
#include "BsOverlayManager.h"
#include "CmApplication.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Application::Application()
	{

	}

	void Application::startUp(const String& renderSystem, const String& renderer, const String& resourceCacheDir)
	{
		CM::START_UP_DESC desc;
		desc.renderSystem = renderSystem;
		desc.renderer= renderer;
		desc.resourceCacheDirectory = resourceCacheDir;

		desc.input = "CamelotOISInput";
		desc.sceneManager = "BansheeOctreeSM";
		desc.importers.push_back("CamelotFreeImgImporter");
		desc.importers.push_back("CamelotFBXImporter");
		desc.importers.push_back("CamelotFontImporter");

		CM::gApplication().startUp(desc);

		OverlayManager::startUp(CM_NEW(OverlayManager, GenAlloc) OverlayManager());
		GUIManager::startUp(CM_NEW(GUIManager, GenAlloc) GUIManager());
		GUIMaterialManager::startUp(CM_NEW(GUIMaterialManager, GenAlloc) GUIMaterialManager());
	}

	void Application::runMainLoop()
	{
		CM::gApplication().runMainLoop();
	}

	void Application::shutDown()
	{
		GUIMaterialManager::instance().forceReleaseAllMaterials();

		CM::gApplication().shutDown();

		GUIMaterialManager::shutDown();
		GUIManager::shutDown();
		OverlayManager::shutDown();
	}

	Application& gBansheeApp()
	{
		static Application application;
		return application;
	}
}