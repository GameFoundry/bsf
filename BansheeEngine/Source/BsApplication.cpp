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
#include "CmProfiler.h"
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

	Application::Application(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer)
		:CoreApplication(createStartUpDesc(primaryWindowDesc, renderSystem, renderer)), mMonoPlugin(nullptr), mSBansheeEnginePlugin(nullptr)
	{
		VirtualInput::startUp();
		ScriptManager::startUp();
		GUIManager::startUp();
		GUIMaterialManager::startUp();
		OverlayManager::startUp();

		BuiltinMaterialManager::startUp();
		BuiltinMaterialManager::instance().addFactory(bs_new<D3D9BuiltinMaterialFactory>());
		BuiltinMaterialManager::instance().addFactory(bs_new<D3D11BuiltinMaterialFactory>());
		BuiltinMaterialManager::instance().addFactory(bs_new<GLBuiltinMaterialFactory>());
		BuiltinMaterialManager::instance().setActive(renderSystem);

		DrawHelper2D::startUp();
		DrawHelper3D::startUp();

		BuiltinResources::startUp();
		Cursor::startUp();

		loadPlugin("BansheeMono", &mMonoPlugin);
		loadPlugin("SBansheeEngine", &mSBansheeEnginePlugin); // Scripting interface
	}

	Application::~Application()
	{
		ScriptManager::instance().destroy();
		unloadPlugin(mSBansheeEnginePlugin);
		unloadPlugin(mMonoPlugin);

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
	}

	void Application::onStartUp()
	{
		CoreApplication::onStartUp();

		Cursor::instance().setCursor(CursorType::Arrow);
	}

	void Application::startUp(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer)
	{
		CoreApplication::startUp<Application>(primaryWindowDesc, renderSystem, renderer);
	}

	void Application::update()
	{
		CoreApplication::update();

		VirtualInput::instance().update();
		PROFILE_CALL(GUIManager::instance().update(), "GUI");
	}

	const ViewportPtr& Application::getPrimaryViewport() const
	{
		// TODO - Need a way to determine primary viewport!
		return nullptr;
	}

	Application& gApplication()
	{
		return static_cast<Application&>(Application::instance());
	}
}