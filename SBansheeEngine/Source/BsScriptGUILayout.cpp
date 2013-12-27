#include "BsScriptGUILayout.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptGUIArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUILayout::ScriptGUILayout(GUILayout* layout, ScriptGUIArea* parentArea)
		:mLayout(layout), mParentArea(parentArea)
	{

	}

	void ScriptGUILayout::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUILayout", &ScriptGUILayout::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}
	void ScriptGUILayout::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromArea", &ScriptGUILayout::internal_createInstanceXFromArea);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromLayout", &ScriptGUILayout::internal_createInstanceXFromLayout);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceYFromLayout", &ScriptGUILayout::internal_createInstanceYFromLayout);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUILayout::internal_destroyInstance);
	}

	void ScriptGUILayout::internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea)
	{
		ScriptGUIArea* scriptArea = ScriptGUIArea::toNative(parentArea);
		GUIArea* nativeArea = scriptArea->getInternalValue();

		ScriptGUILayout* nativeInstance = new (cm_alloc<ScriptGUILayout>()) ScriptGUILayout(&nativeArea->getLayout(), scriptArea);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUILayout::internal_createInstanceXFromLayout(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout& layout = nativeLayout->addLayoutX();

		ScriptGUILayout* nativeInstance = new (cm_alloc<ScriptGUILayout>()) ScriptGUILayout(&layout, scriptLayout->getParentArea());
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUILayout::internal_createInstanceYFromLayout(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout& layout = nativeLayout->addLayoutY();

		ScriptGUILayout* nativeInstance = new (cm_alloc<ScriptGUILayout>()) ScriptGUILayout(&layout, scriptLayout->getParentArea());
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUILayout::internal_destroyInstance(ScriptGUILayout* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}