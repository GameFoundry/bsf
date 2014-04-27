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
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIElementStyle")

		~ScriptGUIElementStyle();

		GUIElementStyle* getInternalValue() const { return mElementStyle; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* name);
		static void internal_createInstanceExternal(MonoObject* instance, MonoString* name, GUIElementStyle* externalStyle);

		CM_SCRIPT_GETSET_OBJECT_SHRDPTR(ScriptGUIElementStyle, ScriptFont, Font, mElementStyle->font, mFont);

		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, FontSize, mElementStyle->fontSize);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, TextHorzAlign, TextHorzAlign, mElementStyle->textHorzAlign);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, TextVertAlign, TextVertAlign, mElementStyle->textVertAlign);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, GUIImagePosition, ImagePosition, mElementStyle->imagePosition);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, bool, WordWrap, mElementStyle->wordWrap);

		CM_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, Normal, mElementStyle->normal, mNormal);
		CM_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, Hover, mElementStyle->hover, mHover);
		CM_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, Active, mElementStyle->active, mActive);
		CM_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, Focused, mElementStyle->focused, mFocused);

		CM_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, NormalOn, mElementStyle->normalOn, mNormalOn);
		CM_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, HoverOn, mElementStyle->hoverOn, mHoverOn);
		CM_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, ActiveOn, mElementStyle->activeOn, mActiveOn);
		CM_SCRIPT_GETSET_OBJECT(ScriptGUIElementStyle, ScriptGUIElementStateStyle, FocusedOn, mElementStyle->focusedOn, mFocusedOn);

		CM_SCRIPT_GETSET_VALUE_REF(ScriptGUIElementStyle, RectOffset, Border, mElementStyle->border);
		CM_SCRIPT_GETSET_VALUE_REF(ScriptGUIElementStyle, RectOffset, Margins, mElementStyle->margins);
		CM_SCRIPT_GETSET_VALUE_REF(ScriptGUIElementStyle, RectOffset, ContentOffset, mElementStyle->contentOffset);

		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, Width, mElementStyle->width);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, Height, mElementStyle->height);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, MinWidth, mElementStyle->minWidth);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, MaxWidth, mElementStyle->maxWidth);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, MinHeight, mElementStyle->minHeight);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, UINT32, MaxHeight, mElementStyle->maxHeight);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, bool, FixedWidth, mElementStyle->fixedWidth);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, bool, FixedHeight, mElementStyle->fixedHeight);

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