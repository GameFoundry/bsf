#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"
#include "BsColor.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIColorField : public TScriptGUIElement<ScriptGUIColorField>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIColorField")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIColorField* nativeInstance, Color* output);
		static void internal_setValue(ScriptGUIColorField* nativeInstance, Color value);

		static void onChanged(MonoObject* instance, Color newValue);

		ScriptGUIColorField(MonoObject* instance, GUIColorField* colorField);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, Color, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}