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
		:TScriptGUIElement(instance, scrollArea)
	{

	}

	void ScriptGUIScrollArea::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIScrollArea::internal_createInstance);
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
}