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

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIScrollArea::ScriptGUIScrollArea(GUIScrollArea* scrollArea)
		:mScrollArea(scrollArea), mIsDestroyed(false)
	{

	}

	void ScriptGUIScrollArea::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUIScrollArea", &ScriptGUIScrollArea::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIScrollArea::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIScrollArea::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIScrollArea::internal_destroyInstance);

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

	void ScriptGUIScrollArea::internal_createInstance(MonoObject* instance, ScrollBarType vertBarType, ScrollBarType horzBarType, 
		MonoObject* scrollBarStyle, MonoObject* scrollAreaStyle, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIElementStyle* scrollAreaNativeStyle = nullptr;

		if(scrollAreaStyle != nullptr)
			scrollAreaNativeStyle = ScriptGUIElementStyle::toNative(scrollAreaStyle)->getInternalValue();

		GUIElementStyle* scrollBarNativeStyle = nullptr;

		if(scrollBarNativeStyle != nullptr)
			scrollBarNativeStyle = ScriptGUIElementStyle::toNative(scrollBarStyle)->getInternalValue();

		GUIScrollArea* guiScrollArea = GUIScrollArea::create(vertBarType, horzBarType, options, scrollBarNativeStyle, scrollAreaNativeStyle);

		ScriptGUIScrollArea* nativeInstance = new (cm_alloc<ScriptGUIScrollArea>()) ScriptGUIScrollArea(guiScrollArea);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
	}

	void ScriptGUIScrollArea::internal_destroyInstance(ScriptGUIScrollArea* nativeInstance)
	{
		nativeInstance->destroy();
		cm_delete(nativeInstance);
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