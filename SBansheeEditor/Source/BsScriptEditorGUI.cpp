#include "BsScriptEditorGUI.h"
#include "BsMonoManager.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsScriptEditorWindow.h"
#include "BsEditorWidget.h"
#include "BsEditorWidgetContainer.h"
#include "BsGUIWidget.h"
#include "BsCamera.h"
#include "CmSceneObject.h"
#include "BsEditorGUI.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	ScriptEditorGUI::ScriptEditorGUI(ScriptEditorWindow* parentWindow)
		:ScriptGUIBase(), mParentWindow(parentWindow)
	{

	}

	void ScriptEditorGUI::initMetaData()
	{
		metaData = ScriptMeta(BansheeEditorAssemblyName, "BansheeEditor", "EditorGUI", &ScriptEditorGUI::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptEditorGUI::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptEditorGUI::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptEditorGUI::internal_destroyInstance);
	}

	void ScriptEditorGUI::internal_createInstance(MonoObject* instance, MonoObject* parentEditorWindow)
	{
		ScriptEditorWindow* nativeParentWindow = ScriptEditorWindow::toNative(parentEditorWindow);

		ScriptEditorGUI* nativeInstance = new (cm_alloc<ScriptEditorGUI>()) ScriptEditorGUI(nativeParentWindow);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, (ScriptGUIBase*)nativeInstance);
	}

	void ScriptEditorGUI::internal_destroyInstance(ScriptEditorGUI* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	GUIWidget& ScriptEditorGUI::getWidget() const
	{
		EditorWidgetContainer* widgetContainer = mParentWindow->getEditorWidget()->_getParent();
		if(widgetContainer == nullptr)
			CM_EXCEPT(InternalErrorException, "Attempting to retrieve GUI widget of an unparented EditorWidget.");

		return widgetContainer->getParentWidget();
	}
}