#pragma once

#include "BsPrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptClass.h"
#include "BsGUIElementStyle.h"
#include "BsScriptMacros.h"
#include "BsScriptSpriteTexture.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptGUIElementStateStyle : public ScriptObject<ScriptGUIElementStateStyle>
	{
	public:
		~ScriptGUIElementStateStyle();

		static void initMetaData();
		GUIElementStyle::GUIElementStateStyle getInternalValue() const { return *mElementStateStyle; }

	private:
		static void internal_createInstance(MonoObject* instance);
		static void internal_createInstanceExternal(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStateStyle);
		static void internal_destroyInstance(ScriptGUIElementStateStyle* nativeInstance);

		static void initRuntimeData()
		{
			metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIElementStateStyle::internal_createInstance);
			metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIElementStateStyle::internal_destroyInstance);

			CM_SCRIPT_SETGET_META(ScriptGUIElementStateStyle, Texture);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStateStyle, TextColor);
		}

		CM_SCRIPT_GETSET_OBJECT_SHRDPTR(ScriptGUIElementStateStyle, ScriptSpriteTexture, Texture, mElementStateStyle->texture, mSpriteTexture);
		CM_SCRIPT_GETSET_VALUE_REF(ScriptGUIElementStateStyle, CM::Color, TextColor, mElementStateStyle->textColor);

		ScriptGUIElementStateStyle();
		ScriptGUIElementStateStyle(GUIElementStyle::GUIElementStateStyle* externalStyle);

		GUIElementStyle::GUIElementStateStyle* mElementStateStyle;
		ScriptSpriteTexture* mSpriteTexture;
		bool mOwnsStyle;
	};
}