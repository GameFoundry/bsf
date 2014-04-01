#include "BsApplication.h"
#include "BsGUIMaterialManager.h"
#include "BsGUIManager.h"
#include "BsOverlayManager.h"
#include "BsDrawHelper2D.h"
#include "BsDrawHelper3D.h"
#include "BsBuiltinMaterialManager.h"
#include "BsD3D9BuiltinMaterialFactory.h"
#include "BsD3D11BuiltinMaterialFactory.h"
#include "BsGLBuiltinMaterialFactory.h"
#include "BsBuiltinResources.h"
#include "BsScriptManager.h"
#include "CmApplication.h"
#include "CmProfiler.h"
#include "BsVirtualInput.h"
#include "BsCursor.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Application::Application()
		:mMonoPlugin(nullptr), mSBansheeEnginePlugin(nullptr)
	{

	}

	void Application::startUp(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer)
	{
		CM::START_UP_DESC desc;
		desc.renderSystem = renderSystem;
		desc.renderer= renderer;
		desc.primaryWindowDesc = primaryWindowDesc;

		desc.input = "CamelotOISInput";
		desc.sceneManager = "BansheeOctreeSM";
		desc.importers.push_back("CamelotFreeImgImporter");
		desc.importers.push_back("CamelotFBXImporter");
		desc.importers.push_back("CamelotFontImporter");
		
		CM::gApplication().startUp(desc);

		VirtualInput::startUp(cm_new<VirtualInput>());
		ScriptManager::startUp(cm_new<ScriptManager>());
		GUIManager::startUp(cm_new<GUIManager>());
		GUIMaterialManager::startUp(cm_new<GUIMaterialManager>());
		OverlayManager::startUp(cm_new<OverlayManager>());

		BuiltinMaterialManager::startUp(cm_new<BuiltinMaterialManager>());
		BuiltinMaterialManager::instance().addFactory(cm_new<D3D9BuiltinMaterialFactory>());
		BuiltinMaterialManager::instance().addFactory(cm_new<D3D11BuiltinMaterialFactory>());
		BuiltinMaterialManager::instance().addFactory(cm_new<GLBuiltinMaterialFactory>());
		BuiltinMaterialManager::instance().setActive(desc.renderSystem);

		DrawHelper2D::startUp(cm_new<DrawHelper2D>());
		DrawHelper3D::startUp(cm_new<DrawHelper3D>());

		BuiltinResources::startUp(cm_new<BuiltinResources>());
		Cursor::startUp(cm_new<Cursor>());

		CM::gApplication().loadPlugin("BansheeMono", &mMonoPlugin);
		CM::gApplication().loadPlugin("SBansheeEngine", &mSBansheeEnginePlugin); // Scripting interface
		
		updateCallbackConn = CM::gApplication().mainLoopCallback.connect(boost::bind(&Application::update, this));

		Cursor::instance().setCursor(CursorType::Arrow);
	}

	void Application::runMainLoop()
	{
		CM::gApplication().runMainLoop();
	}

	void Application::shutDown()
	{
		CM::gApplication().mainLoopCallback.disconnect(updateCallbackConn);

		CM::gApplication().unloadPlugin(mSBansheeEnginePlugin);
		CM::gApplication().unloadPlugin(mMonoPlugin);

		Cursor::shutDown();
		BuiltinResources::shutDown();

		DrawHelper3D::shutDown();
		DrawHelper2D::shutDown();

		GUIMaterialManager::instance().forceReleaseAllMaterials();

		BuiltinMaterialManager::shutDown();

		OverlayManager::shutDown();
		GUIManager::shutDown();
		GUIMaterialManager::shutDown();
		ScriptManager::shutDown();
		VirtualInput::shutDown();
		
		CM::gApplication().shutDown();
	}

	void Application::update()
	{
		VirtualInput::instance().update();
		PROFILE_CALL(GUIManager::instance().update(), "GUI");
	}

	const CM::ViewportPtr& Application::getPrimaryViewport() const
	{
		// TODO - Need a way to determine primary viewport!
		return nullptr;
	}

	Application& gBansheeApp()
	{
		static Application application;
		return application;
	}
}