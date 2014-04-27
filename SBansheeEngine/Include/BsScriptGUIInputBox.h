#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIInputBox : public TScriptGUIElement<ScriptGUIInputBox>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIInputBox")

	private:
		static void internal_createInstance(MonoObject* instance, bool multiline, MonoString* style, MonoArray* guiOptions);

		static void internal_setText(ScriptGUIInputBox* nativeInstance, MonoString* text);
		static void internal_getText(ScriptGUIInputBox* nativeInstance, MonoString** text);

		ScriptGUIInputBox(MonoObject* instance, GUIInputBox* inputBox);
	};
}