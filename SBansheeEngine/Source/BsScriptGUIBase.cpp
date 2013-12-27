#include "BsScriptGUIBase.h"
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
	ScriptGUIBase::ScriptGUIBase(GUIWidget& widget)
		:mWidget(widget)
	{

	}

	void ScriptGUIBase::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIBase", &ScriptGUIBase::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}
	void ScriptGUIBase::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIBase::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIBase::internal_destroyInstance);
	}

	void ScriptGUIBase::internal_createInstance(MonoObject* instance)
	{
		//ScriptGUIArea* scriptArea = ScriptGUIArea::toNative(parentArea);
		//GUIArea* nativeArea = scriptArea->getInternalValue();

		//ScriptGUILayout* nativeInstance = new (cm_alloc<ScriptGUILayout>()) ScriptGUILayout(nativeArea->getLayout(), scriptArea);
		//nativeInstance->createInstance(instance);

		//metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIBase::internal_destroyInstance(ScriptGUIBase* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}