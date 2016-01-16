#include "BsScriptGUI.h"
#include "BsScriptGUISkin.h"
#include "BsScriptGUILayout.h"
#include "BsGUIWidget.h"
#include "BsSceneManager.h"
#include "BsScriptObjectManager.h"
#include "BsBuiltinResources.h"
#include "BsMonoMethod.h"

namespace BansheeEngine
{
	SPtr<GUIWidget> ScriptGUI::sGUIWidget;
	ScriptGUILayout* ScriptGUI::sPanel = nullptr;
	HEvent ScriptGUI::sDomainUnloadConn;
	HEvent ScriptGUI::sDomainLoadConn;
	MonoMethod* ScriptGUI::sGUIPanelMethod = nullptr;

	ScriptGUI::ScriptGUI(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	ScriptGUI::~ScriptGUI()
	{ }

	void ScriptGUI::startUp()
	{
		CameraPtr mainCamera = gSceneManager().getMainCamera().camera;
		sGUIWidget = GUIWidget::create(mainCamera);
		sGUIWidget->setSkin(BuiltinResources::instance().getGUISkin());

		auto createPanel = [] ()
		{
			assert(sPanel == nullptr);

			MonoObject* guiPanel = ScriptGUIPanel::createFromExisting(sGUIWidget->getPanel());
			sPanel = ScriptGUILayout::toNative(guiPanel);

			void* params[1];
			params[0] = guiPanel;

			sGUIPanelMethod->invoke(nullptr, params);
		};

		auto clearPanel = [] ()
		{
			sPanel = nullptr;
		};

		createPanel();

		sDomainLoadConn = ScriptObjectManager::instance().onRefreshDomainLoaded.connect(createPanel);
		sDomainUnloadConn = MonoManager::instance().onDomainUnload.connect(clearPanel);
	}

	void ScriptGUI::update()
	{
		if (sGUIWidget == nullptr)
			return;

		CameraPtr mainCamera = gSceneManager().getMainCamera().camera;
		if (mainCamera != sGUIWidget->getCamera())
			sGUIWidget->setCamera(mainCamera);
	}

	void ScriptGUI::shutDown()
	{
		sDomainLoadConn.disconnect();
		sDomainUnloadConn.disconnect();

		if (sPanel != nullptr)
		{
			sPanel->destroy();
			sPanel = nullptr;
		}

		if (sGUIWidget != nullptr)
		{
			sGUIWidget->_destroy();
			sGUIWidget = nullptr;
		}
	}

	void ScriptGUI::initRuntimeData()
	{
		sGUIPanelMethod = metaData.scriptClass->getMethod("SetPanel", 1);

		metaData.scriptClass->addInternalCall("Internal_SetSkin", &ScriptGUI::internal_SetSkin);
	}

	void ScriptGUI::internal_SetSkin(ScriptGUISkin* skin)
	{
		HGUISkin guiSkin;
		if (skin != nullptr)
			guiSkin = skin->getHandle();

		if (!guiSkin.isLoaded())
			guiSkin = BuiltinResources::instance().getGUISkin();

		if(sGUIWidget != nullptr)
			sGUIWidget->setSkin(guiSkin);
	}
}