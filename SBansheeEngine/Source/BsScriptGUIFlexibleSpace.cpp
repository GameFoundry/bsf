#include "BsScriptGUIFlexibleSpace.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUISpace.h"
#include "BsScriptGUILayout.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIFlexibleSpace::ScriptGUIFlexibleSpace(GUIFlexibleSpace& flexibleSpace, GUILayout* parentLayout)
		:mFlexibleSpace(flexibleSpace), mParentLayout(parentLayout)
	{

	}

	void ScriptGUIFlexibleSpace::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUIFlexibleSpace", &ScriptGUIFlexibleSpace::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIFlexibleSpace::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIFlexibleSpace::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIFlexibleSpace::internal_destroyInstance);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIFlexibleSpace::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_Enable", &ScriptGUIFlexibleSpace::internal_enable);
		metaData.scriptClass->addInternalCall("Internal_Disable", &ScriptGUIFlexibleSpace::internal_disable);
	}

	void ScriptGUIFlexibleSpace::internal_createInstance(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUIFlexibleSpace& space = nativeLayout->addFlexibleSpace();

		ScriptGUIFlexibleSpace* nativeInstance = new (cm_alloc<ScriptGUIFlexibleSpace>()) ScriptGUIFlexibleSpace(space, nativeLayout);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
	}

	void ScriptGUIFlexibleSpace::internal_destroyInstance(ScriptGUIFlexibleSpace* nativeInstance)
	{
		cm_delete(nativeInstance);
	}

	void ScriptGUIFlexibleSpace::internal_destroy(ScriptGUIFlexibleSpace* nativeInstance)
	{
		nativeInstance->mParentLayout->removeFlexibleSpace(nativeInstance->mFlexibleSpace);
	}

	void ScriptGUIFlexibleSpace::internal_disable(ScriptGUIFlexibleSpace* nativeInstance)
	{
		nativeInstance->mFlexibleSpace.disableRecursively();
	}

	void ScriptGUIFlexibleSpace::internal_enable(ScriptGUIFlexibleSpace* nativeInstance)
	{
		nativeInstance->mFlexibleSpace.enableRecursively();
	}
}