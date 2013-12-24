#pragma once

#include "BsPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
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

		/*static MonoObject* internal_GetFont(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetFont(ScriptGUIElementStyle* nativeInstance, ScriptFont* value);

		static int internal_GetFontSize(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetFontSize(ScriptGUIElementStyle* nativeInstance, int value);

		static int internal_GetTextHorzAlign(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetTextHorzAlign(ScriptGUIElementStyle* nativeInstance, int value);

		static int internal_GetTextVertAlign(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetTextVertAlign(ScriptGUIElementStyle* nativeInstance, int value);

		static int internal_GetImagePosition(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetImagePosition(ScriptGUIElementStyle* nativeInstance, int value);

		static bool internal_GetWordWrap(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetWordWrap(ScriptGUIElementStyle* nativeInstance, bool value);


		static MonoObject* internal_GetNormal(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetNormal(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static MonoObject* internal_GetHover(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetHover(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static MonoObject* internal_GetActive(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetActive(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static MonoObject* internal_GetFocused(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetFocused(ScriptGUIElementStyle* nativeInstance, MonoObject* value);


		static MonoObject* internal_GetNormalOn(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetNormalOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static MonoObject* internal_GetHoverOn(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetHoverOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static MonoObject* internal_GetActiveOn(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetActiveOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		static MonoObject* internal_GetFocusedOn(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetFocusedOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value);


		static RectOffset internal_GetBorder(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetBorder(ScriptGUIElementStyle* nativeInstance, RectOffset value);

		static RectOffset internal_GetMargins(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetMargins(ScriptGUIElementStyle* nativeInstance, RectOffset value);

		static RectOffset internal_GetContentOffset(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetContentOffset(ScriptGUIElementStyle* nativeInstance, RectOffset value);


		static INT32 internal_GetWidth(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetWidth(ScriptGUIElementStyle* nativeInstance, INT32 value);

		static INT32 internal_GetHeight(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetHeight(ScriptGUIElementStyle* nativeInstance, INT32 value);

		static INT32 internal_GetMinWidth(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetMinWidth(ScriptGUIElementStyle* nativeInstance, INT32 value);

		static INT32 internal_GetMaxWidth(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetMaxWidth(ScriptGUIElementStyle* nativeInstance, INT32 value);

		static INT32 internal_GetMinHeight(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetMinHeight(ScriptGUIElementStyle* nativeInstance, INT32 value);

		static INT32 internal_GetMaxHeight(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetMaxHeight(ScriptGUIElementStyle* nativeInstance, INT32 value);

		static bool internal_GetFixedWidth(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetFixedWidth(ScriptGUIElementStyle* nativeInstance, bool value);

		static bool internal_GetFixedHeight(ScriptGUIElementStyle* nativeInstance);
		static void internal_SetFixedHeight(ScriptGUIElementStyle* nativeInstance, bool value);*/

		ScriptGUIElementStyle(const CM::String& name);
		ScriptGUIElementStyle(const CM::String& name, GUIElementStyle* externalStyle);

		CM::String mName;
		GUIElementStyle* mElementStyle;
		ScriptFont* mFont;
		bool mOwnsStyle;
	};
}