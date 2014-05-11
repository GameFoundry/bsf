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

namespace BansheeEngine
{
	BsApplication::BsApplication()
		:mMonoPlugin(nullptr), mSBansheeEnginePlugin(nullptr)
	{

	}

	void BsApplication::startUp(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer)
	{
		START_UP_DESC desc;
		desc.renderSystem = renderSystem;
		desc.renderer= renderer;
		desc.primaryWindowDesc = primaryWindowDesc;

		desc.input = "CamelotOISInput";
		desc.sceneManager = "BansheeOctreeSM";
		desc.importers.push_back("CamelotFreeImgImporter");
		desc.importers.push_back("CamelotFBXImporter");
		desc.importers.push_back("CamelotFontImporter");
		
		gApplication().startUp(desc);

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

		gApplication().loadPlugin("BansheeMono", &mMonoPlugin);
		gApplication().loadPlugin("SBansheeEngine", &mSBansheeEnginePlugin); // Scripting interface
		
		updateCallbackConn = gApplication().mainLoopCallback.connect(std::bind(&BsApplication::update, this));

		Cursor::instance().setCursor(CursorType::Arrow);
	}

	void BsApplication::runMainLoop()
	{
		gApplication().runMainLoop();
	}

	void BsApplication::shutDown()
	{
		updateCallbackConn.disconnect();

		ScriptManager::instance().destroy();
		gApplication().unloadPlugin(mSBansheeEnginePlugin);
		gApplication().unloadPlugin(mMonoPlugin);

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
		
		gApplication().shutDown();
	}

	void BsApplication::update()
	{
		VirtualInput::instance().update();
		PROFILE_CALL(GUIManager::instance().update(), "GUI");
	}

	const ViewportPtr& BsApplication::getPrimaryViewport() const
	{
		// TODO - Need a way to determine primary viewport!
		return nullptr;
	}

	BsApplication& gBansheeApp()
	{
		static BsApplication application;
		return application;
	}
}