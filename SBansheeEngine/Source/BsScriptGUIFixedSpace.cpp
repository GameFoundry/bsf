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
	ScriptGUIFixedSpace::ScriptGUIFixedSpace(GUIFixedSpace& fixedSpace)
		:mFixedSpace(fixedSpace)
	{

	}

	void ScriptGUIFixedSpace::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIFixedSpace", &ScriptGUIFixedSpace::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIFixedSpace::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIFixedSpace::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIFixedSpace::internal_destroyInstance);
	}

	void ScriptGUIFixedSpace::internal_createInstance(MonoObject* instance, MonoObject* parentLayout, UINT32 size)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUIFixedSpace& space = nativeLayout->addSpace(size);

		ScriptGUIFixedSpace* nativeInstance = new (cm_alloc<ScriptGUIFixedSpace>()) ScriptGUIFixedSpace(space);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIFixedSpace::internal_destroyInstance(ScriptGUIFixedSpace* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}