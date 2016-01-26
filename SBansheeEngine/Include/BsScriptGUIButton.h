#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIButton : public TScriptGUIElement<ScriptGUIButton>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIButton")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions);
		static void internal_setContent(ScriptGUIButton* nativeInstance, MonoObject* content);

		static void onClick(MonoObject* instance);
		static void onHover(MonoObject* instance);
		static void onOut(MonoObject* instance);

		ScriptGUIButton(MonoObject* instance, GUIButton* button);

		typedef void (__stdcall *OnClickThunkDef) (MonoObject*, MonoException**);
		typedef void (__stdcall *OnHoverThunkDef) (MonoObject*, MonoException**);
		typedef void (__stdcall *OnOutThunkDef) (MonoObject*, MonoException**);

		static OnClickThunkDef onClickThunk;
		static OnHoverThunkDef onHoverThunk;
		static OnOutThunkDef onOutThunk;
	};
}