#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUITextureField : public TScriptGUIElement <ScriptGUITextureField>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUITextureField")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUITextureField* nativeInstance, MonoObject** output);
		static void internal_setValue(ScriptGUITextureField* nativeInstance, MonoObject* value);
		static void internal_setTint(ScriptGUITextureField* nativeInstance, Color color);

		static void onChanged(MonoObject* instance, const HTexture& newValue);
		static MonoObject* nativeToManagedResource(const HTexture& instance);

		ScriptGUITextureField(MonoObject* instance, GUITextureField* textureField);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, MonoObject*, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}