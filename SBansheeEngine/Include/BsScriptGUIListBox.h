#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIListBox : public TScriptGUIElement<ScriptGUIListBox>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIListBox")

	private:
		static void internal_createInstance(MonoObject* instance, MonoArray* elements, MonoString* style, MonoArray* guiOptions);
		static void internal_setElements(ScriptGUIListBox* nativeInstance, MonoArray* elements);
		static void internal_setTint(ScriptGUIListBox* nativeInstance, Color color);

		static void onSelectionChanged(MonoObject* instance, UINT32 index);

		ScriptGUIListBox(MonoObject* instance, GUIListBox* listBox);

		typedef void (__stdcall *OnSelectionChangedThunkDef) (MonoObject*, UINT32, MonoException**);
		static OnSelectionChangedThunkDef onSelectionChangedThunk;
	};
}