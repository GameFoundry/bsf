#include "BsScriptGUILayout.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptGUIArea.h"
#include "BsScriptGUIScrollArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUIScrollArea.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUILayout::ScriptGUILayout(GUILayout* layout, GUIWidget& parentWidget, GUILayout* parentLayout)
		:mLayout(layout), mParentWidget(parentWidget), mParentLayout(parentLayout)
	{

	}

	void ScriptGUILayout::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUILayout", &ScriptGUILayout::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUILayout::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromArea", &ScriptGUILayout::internal_createInstanceXFromArea);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromLayout", &ScriptGUILayout::internal_createInstanceXFromLayout);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceYFromLayout", &ScriptGUILayout::internal_createInstanceYFromLayout);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceYFromScrollArea", &ScriptGUILayout::internal_createInstanceYFromScrollArea);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUILayout::internal_destroyInstance);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUILayout::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_Enable", &ScriptGUILayout::internal_enable);
		metaData.scriptClass->addInternalCall("Internal_Disable", &ScriptGUILayout::internal_disable);
	}

	void ScriptGUILayout::internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea)
	{
		ScriptGUIArea* scriptArea = ScriptGUIArea::toNative(parentArea);
		GUIArea* nativeArea = scriptArea->getInternalValue();

		ScriptGUILayout* nativeInstance = new (cm_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(&nativeArea->getLayout(), scriptArea->getParentWidget(), nullptr);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUILayout::internal_createInstanceXFromLayout(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout& layout = nativeLayout->addLayoutX();

		ScriptGUILayout* nativeInstance = new (cm_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(&layout, scriptLayout->getParentWidget(), nativeLayout);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUILayout::internal_createInstanceYFromLayout(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout& layout = nativeLayout->addLayoutY();

		ScriptGUILayout* nativeInstance = new (cm_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(&layout, scriptLayout->getParentWidget(), nativeLayout);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUILayout::internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea)
	{
		ScriptGUIScrollArea* scriptScrollArea = ScriptGUIScrollArea::toNative(parentScrollArea);
		GUILayout* nativeLayout = &scriptScrollArea->getInternalValue()->getLayout();

		ScriptGUILayout* nativeInstance = new (cm_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(nativeLayout, scriptScrollArea->getParentWidget(), nativeLayout);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUILayout::internal_destroyInstance(ScriptGUILayout* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	void ScriptGUILayout::internal_destroy(ScriptGUILayout* nativeInstance)
	{
		if(nativeInstance->mParentLayout != nullptr)
			nativeInstance->mParentLayout->removeLayout(*nativeInstance->mLayout);
	}

	void ScriptGUILayout::internal_disable(ScriptGUILayout* nativeInstance)
	{
		nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUILayout::internal_enable(ScriptGUILayout* nativeInstance)
	{
		nativeInstance->getInternalValue()->enableRecursively();
	}
}