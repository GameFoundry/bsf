#include "BsScriptGUI.h"
#include "BsScriptGUISkin.h"
#include "BsScriptGUILayout.h"
#include "BsGUIWidget.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	SPtr<GUIWidget> ScriptGUI::sGUIWidget;
	ScriptGUILayout* ScriptGUI::sPanel = nullptr;

	ScriptGUI::ScriptGUI(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	ScriptGUI::~ScriptGUI()
	{ }

	void ScriptGUI::startUp()
	{
		CameraPtr mainCamera = gSceneManager().getMainCamera().camera;
		sGUIWidget = GUIWidget::create(mainCamera);

		MonoObject* guiPanel = ScriptGUIPanel::createFromExisting(sGUIWidget->getPanel());
		sPanel = ScriptGUILayout::toNative(guiPanel);
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
		metaData.scriptClass->addInternalCall("Internal_SetSkin", &ScriptGUI::internal_SetSkin);
		metaData.scriptClass->addInternalCall("Internal_GetPanel", &ScriptGUI::internal_GetPanel);
	}

	void ScriptGUI::internal_SetSkin(ScriptGUISkin* skin)
	{
		HGUISkin guiSkin;
		if (skin != nullptr)
			guiSkin = skin->getHandle();

		if(sGUIWidget != nullptr)
			sGUIWidget->setSkin(guiSkin);
	}

	MonoObject* ScriptGUI::internal_GetPanel()
	{
		if (sPanel != nullptr)
			return sPanel->getManagedInstance();

		return nullptr;
	}
}