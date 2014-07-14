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

namespace BansheeEngine
{
	ScriptGUILayout::ScriptGUILayout(MonoObject* instance, GUILayout* layout, GUILayout* parentLayout)
		:TScriptGUIElementBase(instance, layout), mLayout(layout), mParentLayout(parentLayout), mIsDestroyed(false)
	{

	}

	void ScriptGUILayout::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromArea", &ScriptGUILayout::internal_createInstanceXFromArea);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromLayout", &ScriptGUILayout::internal_createInstanceXFromLayout);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceYFromLayout", &ScriptGUILayout::internal_createInstanceYFromLayout);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceYFromScrollArea", &ScriptGUILayout::internal_createInstanceYFromScrollArea);
	}

	void ScriptGUILayout::destroy()
	{
		if(!mIsDestroyed)
		{
			if(mParentLayout != nullptr)
				mParentLayout->removeLayout(*mLayout);

			mLayout = nullptr;
			mParentLayout = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUILayout::setParent(GUILayout* parentLayout)
	{
		// FixedSpace parent is static, so do nothing
	}

	void ScriptGUILayout::internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea)
	{
		ScriptGUIArea* scriptArea = ScriptGUIArea::toNative(parentArea);
		GUIArea* nativeArea = scriptArea->getInternalValue();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(instance, &nativeArea->getLayout(), nullptr);
	}

	void ScriptGUILayout::internal_createInstanceXFromLayout(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout& layout = nativeLayout->addLayoutX();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(instance, &layout, nativeLayout);
	}

	void ScriptGUILayout::internal_createInstanceYFromLayout(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout& layout = nativeLayout->addLayoutY();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(instance, &layout, nativeLayout);
	}

	void ScriptGUILayout::internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea)
	{
		ScriptGUIScrollArea* scriptScrollArea = ScriptGUIScrollArea::toNative(parentScrollArea);
		GUIScrollArea* scrollArea = (GUIScrollArea*)scriptScrollArea->getGUIElement();

		GUILayout* nativeLayout = &scrollArea->getLayout();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(instance, nativeLayout, nativeLayout);
	}
}