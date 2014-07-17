#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIVector4Field : public TScriptGUIElement<ScriptGUIVector4Field>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIVector4Field")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIVector4Field* nativeInstance, Vector4* output);
		static void internal_setValue(ScriptGUIVector4Field* nativeInstance, Vector4 value);
		static void internal_hasInputFocus(ScriptGUIVector4Field* nativeInstance, bool* output);

		static void onChanged(MonoObject* instance, Vector4 newValue);

		ScriptGUIVector4Field(MonoObject* instance, GUIVector4Field* vector4Field);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, Vector4, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}