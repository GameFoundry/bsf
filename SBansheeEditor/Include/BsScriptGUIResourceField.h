#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIResourceField : public TScriptGUIElement<ScriptGUIResourceField>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIResourceField")

	private:
		static void internal_createInstance(MonoObject* instance, MonoReflectionType* type, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIResourceField* nativeInstance, MonoObject** output);
		static void internal_setValue(ScriptGUIResourceField* nativeInstance, MonoObject* value);

		static void onChanged(MonoObject* instance, const HResource& newValue);
		static MonoObject* nativeToManagedResource(const HResource& instance);

		ScriptGUIResourceField(MonoObject* instance, GUIResourceField* resourceField);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, MonoObject*, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}