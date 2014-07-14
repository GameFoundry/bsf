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

namespace BansheeEngine
{
	ScriptGUIFlexibleSpace::ScriptGUIFlexibleSpace(MonoObject* instance, GUIFlexibleSpace& flexibleSpace, GUILayout* parentLayout)
		:TScriptGUIElementBase(instance, &flexibleSpace), mFlexibleSpace(flexibleSpace), mParentLayout(parentLayout), mIsDestroyed(false)
	{

	}

	void ScriptGUIFlexibleSpace::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIFlexibleSpace::internal_createInstance);
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

	void ScriptGUIFlexibleSpace::setParent(GUILayout* parentLayout)
	{
		// FlexibleSpace parent is static, so do nothing
	}

	void ScriptGUIFlexibleSpace::internal_createInstance(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUIFlexibleSpace& space = nativeLayout->addFlexibleSpace();

		ScriptGUIFlexibleSpace* nativeInstance = new (bs_alloc<ScriptGUIFlexibleSpace>()) ScriptGUIFlexibleSpace(instance, space, nativeLayout);
	}
}