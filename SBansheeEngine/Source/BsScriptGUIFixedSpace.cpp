#include "BsScriptGUIFixedSpace.h"
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
	ScriptGUIFixedSpace::ScriptGUIFixedSpace(GUIFixedSpace& fixedSpace, GUILayout* parentLayout)
		:mFixedSpace(fixedSpace), mParentLayout(parentLayout)
	{

	}

	void ScriptGUIFixedSpace::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUIFixedSpace", &ScriptGUIFixedSpace::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIFixedSpace::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIFixedSpace::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIFixedSpace::internal_destroyInstance);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIFixedSpace::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_Enable", &ScriptGUIFixedSpace::internal_enable);
		metaData.scriptClass->addInternalCall("Internal_Disable", &ScriptGUIFixedSpace::internal_disable);
	}

	void ScriptGUIFixedSpace::internal_createInstance(MonoObject* instance, MonoObject* parentLayout, UINT32 size)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUIFixedSpace& space = nativeLayout->addSpace(size);

		ScriptGUIFixedSpace* nativeInstance = new (cm_alloc<ScriptGUIFixedSpace>()) ScriptGUIFixedSpace(space, nativeLayout);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
	}

	void ScriptGUIFixedSpace::internal_destroyInstance(ScriptGUIFixedSpace* nativeInstance)
	{
		cm_delete(nativeInstance);
	}

	void ScriptGUIFixedSpace::internal_destroy(ScriptGUIFixedSpace* nativeInstance)
	{
		nativeInstance->mParentLayout->removeSpace(nativeInstance->mFixedSpace);
	}

	void ScriptGUIFixedSpace::internal_disable(ScriptGUIFixedSpace* nativeInstance)
	{
		nativeInstance->mFixedSpace.disableRecursively();
	}

	void ScriptGUIFixedSpace::internal_enable(ScriptGUIFixedSpace* nativeInstance)
	{
		nativeInstance->mFixedSpace.enableRecursively();
	}
}