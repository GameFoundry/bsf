#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsMonoClass.h"
#include "BsGUIElementStyle.h"
#include "BsScriptMacros.h"
#include "BsScriptSpriteTexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIElementStateStyle : public ScriptObject<ScriptGUIElementStateStyle>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIElementStateStyle")

		~ScriptGUIElementStateStyle();

		GUIElementStyle::GUIElementStateStyle getInternalValue() const { return *mElementStateStyle; }

	private:
		static void internal_createInstance(MonoObject* instance);
		static void internal_createInstanceExternal(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStateStyle);

		BS_SCRIPT_GETSET_OBJECT_SHRDPTR(ScriptGUIElementStateStyle, ScriptSpriteTexture, Texture, mElementStateStyle->texture, mSpriteTexture);
		BS_SCRIPT_GETSET_VALUE_REF(ScriptGUIElementStateStyle, Color, TextColor, mElementStateStyle->textColor);

		ScriptGUIElementStateStyle(MonoObject* instance);
		ScriptGUIElementStateStyle(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStyle);

		GUIElementStyle::GUIElementStateStyle* mElementStateStyle;
		ScriptSpriteTexture* mSpriteTexture;
		bool mOwnsStyle;
	};
}