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
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

namespace BansheeEngine
{
	ScriptGUIScrollArea::ScriptGUIScrollArea(MonoObject* instance, GUIScrollArea* scrollArea)
		:TScriptGUIElement(instance, scrollArea)
	{

	}

	void ScriptGUIScrollArea::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIScrollArea::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetContentBounds", &ScriptGUIScrollArea::internal_getContentBounds);
		metaData.scriptClass->addInternalCall("Internal_GetHorzScroll", &ScriptGUIScrollArea::internal_getHorzScroll);
		metaData.scriptClass->addInternalCall("Internal_SetHorzScroll", &ScriptGUIScrollArea::internal_setHorzScroll);
		metaData.scriptClass->addInternalCall("Internal_GetVertScroll", &ScriptGUIScrollArea::internal_getVertScroll);
		metaData.scriptClass->addInternalCall("Internal_SetVertScroll", &ScriptGUIScrollArea::internal_setVertScroll);
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

		ScriptGUIScrollArea* nativeInstance = new (bs_alloc<ScriptGUIScrollArea>()) ScriptGUIScrollArea(instance, guiScrollArea);
	}

	Rect2I ScriptGUIScrollArea::internal_getContentBounds(ScriptGUIScrollArea* nativeInstance)
	{
		GUIScrollArea* guiScrollArea = static_cast<GUIScrollArea*>(nativeInstance->getGUIElement());
		return guiScrollArea->getContentBounds();
	}

	float ScriptGUIScrollArea::internal_getHorzScroll(ScriptGUIScrollArea* nativeInstance)
	{
		GUIScrollArea* guiScrollArea = static_cast<GUIScrollArea*>(nativeInstance->getGUIElement());
		return guiScrollArea->getHorizontalScroll();
	}

	void ScriptGUIScrollArea::internal_setHorzScroll(ScriptGUIScrollArea* nativeInstance, float value)
	{
		GUIScrollArea* guiScrollArea = static_cast<GUIScrollArea*>(nativeInstance->getGUIElement());
		guiScrollArea->scrollToHorizontal(value);
	}

	float ScriptGUIScrollArea::internal_getVertScroll(ScriptGUIScrollArea* nativeInstance)
	{
		GUIScrollArea* guiScrollArea = static_cast<GUIScrollArea*>(nativeInstance->getGUIElement());
		return guiScrollArea->getVerticalScroll();
	}

	void ScriptGUIScrollArea::internal_setVertScroll(ScriptGUIScrollArea* nativeInstance, float value)
	{
		GUIScrollArea* guiScrollArea = static_cast<GUIScrollArea*>(nativeInstance->getGUIElement());
		guiScrollArea->scrollToVertical(value);
	}
}