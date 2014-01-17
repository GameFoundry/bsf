#include "BsScriptEditorGUI.h"
#include "BsMonoManager.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsScriptModalWindow.h"
#include "BsGUIWidget.h"
#include "BsCamera.h"
#include "CmSceneObject.h"
#include "BsEditorGUI.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	ScriptEditorGUI::ScriptEditorGUI(GUIWidget& widget, ScriptModalWindow* parentWindow)
		:ScriptGUIBase(widget)
	{

	}

	void ScriptEditorGUI::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEditor", "BansheeEditor", "EditorGUI", &ScriptEditorGUI::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptEditorGUI::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptEditorGUI::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptEditorGUI::internal_destroyInstance);
	}

	void ScriptEditorGUI::internal_createInstance(MonoObject* instance, MonoObject* parentModalWindow)
	{
		ScriptModalWindow* nativeParentWindow = ScriptModalWindow::toNative(parentModalWindow);

		HSceneObject sceneObj = nativeParentWindow->getSceneObject();
		HGUIWidget widget = sceneObj->addComponent<GUIWidget>(nativeParentWindow->getCamera()->getViewport().get());
		widget->setSkin(EditorGUI::instance().getSkin());
		widget->setDepth(128);

		ScriptEditorGUI* nativeInstance = new (cm_alloc<ScriptEditorGUI>()) ScriptEditorGUI(*widget.get(), nativeParentWindow);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, (ScriptGUIBase*)nativeInstance);
	}

	void ScriptEditorGUI::internal_destroyInstance(ScriptEditorGUI* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}