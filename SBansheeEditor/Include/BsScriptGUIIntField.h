#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIIntField : public TScriptGUIElement<ScriptGUIIntField>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIIntField")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth, MonoString* titleStyle, 
			MonoString* inputStyle, MonoArray* guiOptions, bool withTitle);

		static INT32 internal_getValue(ScriptGUIIntField* nativeInstance);
		static void internal_setValue(ScriptGUIIntField* nativeInstance, INT32 value);

		static void onChanged(MonoObject* instance, INT32 newValue);

		ScriptGUIIntField(MonoObject* instance, GUIIntField* intField);

		typedef void (__stdcall *OnChangedThunkDef) (MonoObject*, INT32, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}