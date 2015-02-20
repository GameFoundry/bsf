#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIVector3Field : public TScriptGUIElement<ScriptGUIVector3Field>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIVector3Field")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIVector3Field* nativeInstance, Vector3* output);
		static void internal_setValue(ScriptGUIVector3Field* nativeInstance, Vector3 value);
		static void internal_hasInputFocus(ScriptGUIVector3Field* nativeInstance, bool* output);
		static void internal_setTint(ScriptGUIVector3Field* nativeInstance, Color color);

		static void onChanged(MonoObject* instance, Vector3 newValue);

		ScriptGUIVector3Field(MonoObject* instance, GUIVector3Field* vector3Field);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, Vector3, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}