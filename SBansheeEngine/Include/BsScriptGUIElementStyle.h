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
		~ScriptGUIElementStyle();

		static void initMetaData();
		GUIElementStyle* getInternalValue() const { return mElementStyle; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* name);
		static void internal_createInstanceExternal(MonoObject* instance, MonoString* name, GUIElementStyle* externalStyle);
		static void internal_destroyInstance(ScriptGUIElementStyle* nativeInstance);

		static void initRuntimeData()
		{
			metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIElementStyle::internal_createInstance);
			metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIElementStyle::internal_destroyInstance);

			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, Font);

			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, FontSize);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, TextHorzAlign);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, TextVertAlign);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, ImagePosition);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, WordWrap);

			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, Normal);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, Hover);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, Active);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, Focused);

			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, NormalOn);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, HoverOn);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, ActiveOn);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, FocusedOn);

			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, Border);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, Margins);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, ContentOffset);

			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, Width);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, Height);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, MinWidth);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, MaxWidth);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, MinHeight);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, MaxHeight);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, FixedWidth);
			CM_SCRIPT_SETGET_META(ScriptGUIElementStyle, FixedHeight);
		}

		CM_SCRIPT_GETSET_OBJECT_SHRDPTR(ScriptGUIElementStyle, ScriptFont, Font, mElementStyle->font, mFont);

		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, CM::UINT32, FontSize, mElementStyle->fontSize);
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

		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, CM::UINT32, Width, mElementStyle->width);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, CM::UINT32, Height, mElementStyle->height);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, CM::UINT32, MinWidth, mElementStyle->minWidth);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, CM::UINT32, MaxWidth, mElementStyle->maxWidth);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, CM::UINT32, MinHeight, mElementStyle->minHeight);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, CM::UINT32, MaxHeight, mElementStyle->maxHeight);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, bool, FixedWidth, mElementStyle->fixedWidth);
		CM_SCRIPT_GETSET_VALUE(ScriptGUIElementStyle, bool, FixedHeight, mElementStyle->fixedHeight);

		ScriptGUIElementStyle(const CM::String& name);
		ScriptGUIElementStyle(const CM::String& name, GUIElementStyle* externalStyle);

		CM::String mName;
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