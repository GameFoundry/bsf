#include "BsScriptGUIScrollArea.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIScrollArea.h"
#include "BsGUIOptions.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

namespace BansheeEngine
{
	ScriptGUIScrollArea::ScriptGUIScrollArea(MonoObject* instance, GUIScrollArea* scrollArea)
		:ScriptObject(instance), mScrollArea(scrollArea), mIsDestroyed(false)
	{

	}

	void ScriptGUIScrollArea::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIScrollArea::internal_createInstance);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIScrollArea::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUIScrollArea::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUIScrollArea::internal_setParent);
	}

	void ScriptGUIScrollArea::destroy()
	{
		if(!mIsDestroyed)
		{
			GUIElement::destroy(mScrollArea);
			mScrollArea = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUIScrollArea::_onManagedInstanceDeleted()
	{
		destroy();

		ScriptObject::_onManagedInstanceDeleted();
	}

	void ScriptGUIScrollArea::internal_createInstance(MonoObject* instance, ScrollBarType vertBarType, ScrollBarType horzBarType, 
		MonoString* scrollBarStyle, MonoString* scrollAreaStyle, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIScrollArea* guiScrollArea = GUIScrollArea::create(vertBarType, horzBarType, options, 
			toString(MonoUtil::monoToWString(scrollBarStyle)), toString(MonoUtil::monoToWString(scrollAreaStyle)));

		ScriptGUIScrollArea* nativeInstance = new (cm_alloc<ScriptGUIScrollArea>()) ScriptGUIScrollArea(instance, guiScrollArea);
	}

	void ScriptGUIScrollArea::internal_destroy(ScriptGUIScrollArea* nativeInstance)
	{
		nativeInstance->destroy();
	}

	void ScriptGUIScrollArea::internal_setVisible(ScriptGUIScrollArea* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->getInternalValue()->enableRecursively();
		else
			nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUIScrollArea::internal_setParent(ScriptGUIScrollArea* nativeInstance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(nativeInstance->getInternalValue());
	}
}