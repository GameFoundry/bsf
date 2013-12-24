#include "BsScriptGUIWidget.h"
#include "BsScriptMeta.h"
#include "BsScriptField.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
#include "CmSceneObject.h"
#include "BsGUIWidget.h"
#include "BsApplication.h"
#include "BsScriptComponent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIWidget::ScriptGUIWidget(const HGUIWidget& widget)
		:mWidget(widget)
	{

	}

	void ScriptGUIWidget::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIWidget", &ScriptGUIWidget::initRuntimeData);

		ScriptManager::registerScriptType(&metaData);
	}

	void ScriptGUIWidget::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIWidget::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIWidget::internal_destroyInstance);
	}

	void ScriptGUIWidget::internal_createInstance(MonoObject* instance, SceneObject* sceneObject)
	{
		HGUIWidget widget = sceneObject->addComponent<GUIWidget>(gBansheeApp().getPrimaryViewport().get());
		sceneObject->addComponent<ScriptComponent>(instance);

		ScriptGUIWidget* nativeInstance = new (cm_alloc<ScriptGUIWidget>()) ScriptGUIWidget(widget);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIWidget::internal_destroyInstance(ScriptGUIWidget* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}