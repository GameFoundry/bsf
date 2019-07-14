//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Wrappers/GUI/BsScriptGUI.h"
#include "Wrappers/GUI/BsScriptGUILayout.h"
#include "GUI/BsGUIWidget.h"
#include "Scene/BsSceneManager.h"
#include "BsScriptObjectManager.h"
#include "Resources/BsBuiltinResources.h"
#include "BsMonoMethod.h"

#include "Generated/BsScriptGUISkin.generated.h"

namespace bs
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
		SPtr<Camera> mainCamera = gSceneManager().getMainCamera();
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

		SPtr<Camera> mainCamera = gSceneManager().getMainCamera();
		if (mainCamera != sGUIWidget->getCamera())
			sGUIWidget->setCamera(mainCamera);

		sGUIWidget->_updateRT();
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

		metaData.scriptClass->addInternalCall("Internal_SetSkin", (void*)&ScriptGUI::internal_SetSkin);
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
