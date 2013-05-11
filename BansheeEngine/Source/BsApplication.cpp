#include "BsApplication.h"
#include "BsGUIMaterialManager.h"
#include "BsGUIManager.h"
#include "BsOverlayManager.h"
#include "BsBuiltinMaterialManager.h"
#include "BsD3D9BuiltinMaterialFactory.h"
#include "BsD3D11BuiltinMaterialFactory.h"
#include "BsGLBuiltinMaterialFactory.h"
#include "BsEngineGUI.h"
#include "CmApplication.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Application::Application()
	{

	}

	void Application::startUp(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer, const String& resourceCacheDir)
	{
		CM::START_UP_DESC desc;
		desc.renderSystem = renderSystem;
		desc.renderer= renderer;
		desc.resourceCacheDirectory = resourceCacheDir;
		desc.primaryWindowDesc = primaryWindowDesc;

		desc.input = "CamelotOISInput";
		desc.sceneManager = "BansheeOctreeSM";
		desc.importers.push_back("CamelotFreeImgImporter");
		desc.importers.push_back("CamelotFBXImporter");
		desc.importers.push_back("CamelotFontImporter");

		GUIManager::startUp(CM_NEW(GUIManager, GenAlloc) GUIManager());
		GUIMaterialManager::startUp(CM_NEW(GUIMaterialManager, GenAlloc) GUIMaterialManager());
		OverlayManager::startUp(CM_NEW(OverlayManager, GenAlloc) OverlayManager());

		CM::gApplication().startUp(desc);

		BuiltinMaterialManager::startUp(CM_NEW(BuiltinMaterialManager, GenAlloc) BuiltinMaterialManager());
		BuiltinMaterialManager::instance().addFactory(CM_NEW(D3D9BuiltinMaterialFactory, GenAlloc) D3D9BuiltinMaterialFactory());
		BuiltinMaterialManager::instance().addFactory(CM_NEW(D3D11BuiltinMaterialFactory, GenAlloc) D3D11BuiltinMaterialFactory());
		BuiltinMaterialManager::instance().addFactory(CM_NEW(GLBuiltinMaterialFactory, GenAlloc) GLBuiltinMaterialFactory());
		BuiltinMaterialManager::instance().setActive(desc.renderSystem);

		EngineGUI::startUp(new EngineGUI());

		updateCallbackConn = CM::gApplication().mainLoopCallback.connect(boost::bind(&Application::update, this));
	}

	void Application::runMainLoop()
	{
		CM::gApplication().runMainLoop();
	}

	void Application::shutDown()
	{
		CM::gApplication().mainLoopCallback.disconnect(updateCallbackConn);

		EngineGUI::shutDown();

		GUIMaterialManager::instance().forceReleaseAllMaterials();

		BuiltinMaterialManager::shutDown();

		CM::gApplication().shutDown();

		OverlayManager::shutDown();
		GUIMaterialManager::shutDown();
		GUIManager::shutDown();
	}

	void Application::update()
	{
		GUIManager::instance().update();
	}

	Application& gBansheeApp()
	{
		static Application application;
		return application;
	}
}