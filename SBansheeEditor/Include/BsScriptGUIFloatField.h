#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIFloatField : public TScriptGUIElement<ScriptGUIFloatField>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIFloatField")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIFloatField* nativeInstance, float* output);
		static void internal_setValue(ScriptGUIFloatField* nativeInstance, float value);
		static void internal_hasInputFocus(ScriptGUIFloatField* nativeInstance, bool* output);

		static void onChanged(MonoObject* instance, float newValue);

		ScriptGUIFloatField(MonoObject* instance, GUIFloatField* floatField);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, float, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}