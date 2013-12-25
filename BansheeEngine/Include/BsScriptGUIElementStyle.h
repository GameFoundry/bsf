#pragma once

#include "BsPrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIElementStyle.h"

namespace BansheeEngine
{
	template<class ParentType, class Type>
	class script_getset_value
	{
	private:
		static void internal_get(ParentType* nativeInstance, Type* value);
		static void internal_set(ParentType* nativeInstance, Type value);
	};

	class BS_EXPORT ScriptGUIElementStyle : public ScriptObject<ScriptGUIElementStyle>
	{
	public:
		~ScriptGUIElementStyle();

		static void initMetaData();

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* name);
		static void internal_createInstanceExternal(MonoObject* instance, MonoString* name, GUIElementStyle* externalStyle);
		static void internal_destroyInstance(ScriptGUIElementStyle* nativeInstance);

		static void initRuntimeData();

		static void internal_GetFont(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetFont(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static void internal_GetFontSize(ScriptGUIElementStyle* nativeInstance, CM::UINT32* value);
		static void internal_SetFontSize(ScriptGUIElementStyle* nativeInstance, CM::UINT32 value);

		static void internal_GetTextHorzAlign(ScriptGUIElementStyle* nativeInstance, TextHorzAlign* value);
		static void internal_SetTextHorzAlign(ScriptGUIElementStyle* nativeInstance, TextHorzAlign value);

		static void internal_GetTextVertAlign(ScriptGUIElementStyle* nativeInstance, TextVertAlign* value);
		static void internal_SetTextVertAlign(ScriptGUIElementStyle* nativeInstance, TextVertAlign value);

		static void internal_GetImagePosition(ScriptGUIElementStyle* nativeInstance, GUIImagePosition* value);
		static void internal_SetImagePosition(ScriptGUIElementStyle* nativeInstance, GUIImagePosition value);

		static void internal_GetWordWrap(ScriptGUIElementStyle* nativeInstance, bool* value);
		static void internal_SetWordWrap(ScriptGUIElementStyle* nativeInstance, bool value);


		static void internal_GetNormal(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetNormal(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static void internal_GetHover(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetHover(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static void internal_GetActive(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetActive(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static void internal_GetFocused(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetFocused(ScriptGUIElementStyle* nativeInstance, MonoObject* value);


		static void internal_GetNormalOn(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetNormalOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static void internal_GetHoverOn(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetHoverOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static void internal_GetActiveOn(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetActiveOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static void internal_GetFocusedOn(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetFocusedOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value);


		static void internal_GetBorder(ScriptGUIElementStyle* nativeInstance, RectOffset* value);
		static void internal_SetBorder(ScriptGUIElementStyle* nativeInstance, RectOffset* value);

		static void internal_GetMargins(ScriptGUIElementStyle* nativeInstance, RectOffset* value);
		static void internal_SetMargins(ScriptGUIElementStyle* nativeInstance, RectOffset* value);

		static void internal_GetContentOffset(ScriptGUIElementStyle* nativeInstance, RectOffset* value);
		static void internal_SetContentOffset(ScriptGUIElementStyle* nativeInstance, RectOffset* value);


		static void internal_GetWidth(ScriptGUIElementStyle* nativeInstance, CM::UINT32* value);
		static void internal_SetWidth(ScriptGUIElementStyle* nativeInstance, CM::UINT32 value);

		static void internal_GetHeight(ScriptGUIElementStyle* nativeInstance, CM::UINT32* value);
		static void internal_SetHeight(ScriptGUIElementStyle* nativeInstance, CM::UINT32 value);

		static void internal_GetMinWidth(ScriptGUIElementStyle* nativeInstance, CM::UINT32* value);
		static void internal_SetMinWidth(ScriptGUIElementStyle* nativeInstance, CM::UINT32 value);

		static void internal_GetMaxWidth(ScriptGUIElementStyle* nativeInstance, CM::UINT32* value);
		static void internal_SetMaxWidth(ScriptGUIElementStyle* nativeInstance, CM::UINT32 value);

		static void internal_GetMinHeight(ScriptGUIElementStyle* nativeInstance, CM::UINT32* value);
		static void internal_SetMinHeight(ScriptGUIElementStyle* nativeInstance, CM::UINT32 value);

		static void internal_GetMaxHeight(ScriptGUIElementStyle* nativeInstance, CM::UINT32* value);
		static void internal_SetMaxHeight(ScriptGUIElementStyle* nativeInstance, CM::UINT32 value);

		static void internal_GetFixedWidth(ScriptGUIElementStyle* nativeInstance, bool* value);
		static void internal_SetFixedWidth(ScriptGUIElementStyle* nativeInstance, bool value);

		static void internal_GetFixedHeight(ScriptGUIElementStyle* nativeInstance, bool* value);
		static void internal_SetFixedHeight(ScriptGUIElementStyle* nativeInstance, bool value);

		ScriptGUIElementStyle(const CM::String& name);
		ScriptGUIElementStyle(const CM::String& name, GUIElementStyle* externalStyle);

		CM::String mName;
		GUIElementStyle* mElementStyle;
		bool mOwnsStyle;

		ScriptFont* mFont;
		ScriptGUIElementStyle* mNormal;
		ScriptGUIElementStyle* mHover;
		ScriptGUIElementStyle* mActive;
		ScriptGUIElementStyle* mFocused;
		ScriptGUIElementStyle* mNormalOn;
		ScriptGUIElementStyle* mHoverOn;
		ScriptGUIElementStyle* mActiveOn;
		ScriptGUIElementStyle* mFocusedOn;
	};
}