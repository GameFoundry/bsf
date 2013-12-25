#pragma once

#include "BsPrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIElementStyle.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptGUIElementStateStyle : public ScriptObject<ScriptGUIElementStateStyle>
	{
	public:
		~ScriptGUIElementStateStyle();

		static void initMetaData();
		GUIElementStyle::GUIElementStateStyle* getInternalValue() const { return mElementStateStyle; }

	private:
		static void internal_createInstance(MonoObject* instance);
		static void internal_createInstanceExternal(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStateStyle);
		static void internal_destroyInstance(ScriptGUIElementStateStyle* nativeInstance);

		static void initRuntimeData();

		static void internal_GetTexture(ScriptGUIElementStateStyle* nativeInstance, MonoObject** value);
		static void internal_SetTexture(ScriptGUIElementStateStyle* nativeInstance, MonoObject* value);

		static void internal_GetTextColor(ScriptGUIElementStateStyle* nativeInstance, CM::Color* value);
		static void internal_SetTextColor(ScriptGUIElementStateStyle* nativeInstance, CM::Color* value);

		ScriptGUIElementStateStyle();
		ScriptGUIElementStateStyle(GUIElementStyle::GUIElementStateStyle* externalStyle);

		GUIElementStyle::GUIElementStateStyle* mElementStateStyle;
		ScriptSpriteTexture* mSpriteTexture;
		bool mOwnsStyle;
	};
}