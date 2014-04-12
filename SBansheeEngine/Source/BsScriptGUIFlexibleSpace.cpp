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
		:mFlexibleSpace(flexibleSpace), mParentLayout(parentLayout), mIsDestroyed(false)
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
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUIFlexibleSpace::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUIFlexibleSpace::internal_setParent);
	}

	void ScriptGUIFlexibleSpace::destroy()
	{
		if(!mIsDestroyed)
		{
			mParentLayout->removeFlexibleSpace(mFlexibleSpace);
			mParentLayout = nullptr;

			mIsDestroyed = true;
		}
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
		nativeInstance->destroy();
		cm_delete(nativeInstance);
	}

	void ScriptGUIFlexibleSpace::internal_destroy(ScriptGUIFlexibleSpace* nativeInstance)
	{
		nativeInstance->destroy();
	}

	void ScriptGUIFlexibleSpace::internal_setVisible(ScriptGUIFlexibleSpace* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->mFlexibleSpace.enableRecursively();
		else
			nativeInstance->mFlexibleSpace.disableRecursively();
	}

	void ScriptGUIFlexibleSpace::internal_setParent(ScriptGUIFlexibleSpace* nativeInstance, MonoObject* parentLayout)
	{
		// FlexibleSpace parent is static, so do nothing
	}
}