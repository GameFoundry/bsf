#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIElementStyle.h"
#include "BsScriptMacros.h"
#include "BsMonoClass.h"
#include "BsScriptGUIElementStateStyle.h"
#include "BsScriptFont.h"

namespace BansheeEngine
{
	template<class ParentType, class Type>
	class script_getset_value
	{
	private:
		static void internal_get(ParentType* nativeInstance, Type* value);
		static void internal_set(ParentType* nativeInstance, Type value);
	};

	class BS_SCR_BE_EXPORT ScriptGUIElementStyle : public ScriptObject<ScriptGUIElementStyle>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIElementStyle")

		~ScriptGUIElementStyle();

		GUIElementStyle* getInternalValue() const { return mElementStyle; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* name);
		static void internal_createInstanceExternal(MonoObject* instance, MonoString* name, GUIElementStyle* externalStyle);
		static void internal_addSubStyle(ScriptGUIElementStyle* nativeInstance, MonoString* guiType, MonoString* styleName);

		static void internal_GetFont(ScriptGUIElementStyle* nativeInstance, MonoObject** value);
		static void internal_SetFont(ScriptGUIElementStyle* nativeInstance, MonoObject* value);

		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, FontSize, mElementStyle->fontSize);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, TextHorzAlign, TextHorzAlign, mElementStyle->textHorzAlign);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, TextVertAlign, TextVertAlign, mElementStyle->textVertAlign);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, GUIImagePosition, ImagePosition, mElementStyle->imagePosition);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, bool, WordWrap, mElementStyle->wordWrap);

		BS_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, Normal, mElementStyle->normal, mNormal);
		BS_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, Hover, mElementStyle->hover, mHover);
		BS_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, Active, mElementStyle->active, mActive);
		BS_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, Focused, mElementStyle->focused, mFocused);

		BS_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, NormalOn, mElementStyle->normalOn, mNormalOn);
		BS_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, HoverOn, mElementStyle->hoverOn, mHoverOn);
		BS_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, ActiveOn, mElementStyle->activeOn, mActiveOn);
		BS_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, FocusedOn, mElementStyle->focusedOn, mFocusedOn);

		BS_SCRIPT_GETSET_VALUE_REF(ScriptGUIElementStyle, RectOffset, Border, mElementStyle->border);
		BS_SCRIPT_GETSET_VALUE_REF(ScriptGUIElementStyle, RectOffset, Margins, mElementStyle->margins);
		BS_SCRIPT_GETSET_VALUE_REF(ScriptGUIElementStyle, RectOffset, ContentOffset, mElementStyle->contentOffset);

		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, Width, mElementStyle->width);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, Height, mElementStyle->height);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, MinWidth, mElementStyle->minWidth);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, MaxWidth, mElementStyle->maxWidth);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, MinHeight, mElementStyle->minHeight);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, MaxHeight, mElementStyle->maxHeight);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, bool, FixedWidth, mElementStyle->fixedWidth);
		BS_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, bool, FixedHeight, mElementStyle->fixedHeight);

		ScriptGUIElementStyle(MonoObject* instance, const String& name);
		ScriptGUIElementStyle(MonoObject* instance, const String& name, GUIElementStyle* externalStyle);

		String mName;
		GUIElementStyle* mElementStyle;
		bool mOwnsStyle;

		ScriptFont* mFont;
		ScriptGUIElementStateStyle* mNormal;
		ScriptGUIElementStateStyle* mHover;
		ScriptGUIElementStateStyle* mActive;
		ScriptGUIElementStateStyle* mFocused;
		ScriptGUIElementStateStyle* mNormalOn;
		ScriptGUIElementStateStyle* mHoverOn;
		ScriptGUIElementStateStyle* mActiveOn;
		ScriptGUIElementStateStyle* mFocusedOn;
	};
}