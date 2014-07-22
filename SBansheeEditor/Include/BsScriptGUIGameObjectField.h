#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIGameObjectField : public TScriptGUIElement<ScriptGUIGameObjectField>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIGameObjectField")

	private:
		static void internal_createInstance(MonoObject* instance, MonoReflectionType* type, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIGameObjectField* nativeInstance, MonoObject** output);
		static void internal_setValue(ScriptGUIGameObjectField* nativeInstance, MonoObject* value);

		static void onChanged(MonoObject* instance, const HGameObject& newValue);
		static MonoObject* nativeToManagedGO(const HGameObject& instance);

		ScriptGUIGameObjectField(MonoObject* instance, GUIGameObjectField* GOField);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, MonoObject*, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}