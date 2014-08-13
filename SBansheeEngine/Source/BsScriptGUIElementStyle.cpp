#include "BsScriptGUIElementStyle.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptFont.h"
#include "BsException.h"
#include "BsGUIElementStyle.h"
#include "BsScriptGUIElementStateStyle.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptGUIElementStyle::ScriptGUIElementStyle(MonoObject* instance, const String& name)
		:ScriptObject(instance), mName(name), mElementStyle(bs_new<GUIElementStyle>()), mFont(nullptr), mOwnsStyle(true), mNormal(nullptr), mHover(nullptr),
		mActive(nullptr), mFocused(nullptr), mNormalOn(nullptr), mHoverOn(nullptr), mActiveOn(nullptr), mFocusedOn(nullptr)
	{

	}

	ScriptGUIElementStyle::ScriptGUIElementStyle(MonoObject* instance, const String& name, GUIElementStyle* externalStyle)
		:ScriptObject(instance), mName(name), mElementStyle(externalStyle), mFont(nullptr), mOwnsStyle(false), mNormal(nullptr), mHover(nullptr),
		mActive(nullptr), mFocused(nullptr), mNormalOn(nullptr), mHoverOn(nullptr), mActiveOn(nullptr), mFocusedOn(nullptr)
	{

	}

	ScriptGUIElementStyle::~ScriptGUIElementStyle()
	{
		if(mOwnsStyle)
			bs_delete(mElementStyle);
	}

	void ScriptGUIElementStyle::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIElementStyle::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_AddSubStyle", &ScriptGUIElementStyle::internal_addSubStyle);

		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, Font);

		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, FontSize);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, TextHorzAlign);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, TextVertAlign);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, ImagePosition);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, WordWrap);

		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, Normal);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, Hover);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, Active);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, Focused);

		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, NormalOn);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, HoverOn);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, ActiveOn);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, FocusedOn);

		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, Border);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, Margins);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, ContentOffset);

		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, Width);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, Height);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, MinWidth);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, MaxWidth);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, MinHeight);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, MaxHeight);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, FixedWidth);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStyle, FixedHeight);
	}

	void ScriptGUIElementStyle::internal_createInstance(MonoObject* instance, MonoString* name)
	{
		char* nativeName = mono_string_to_utf8(name);
		String styleName(nativeName);
		free(nativeName);

		ScriptGUIElementStyle* nativeInstance = new (bs_alloc<ScriptGUIElementStyle>()) ScriptGUIElementStyle(instance, styleName);
	}

	void ScriptGUIElementStyle::internal_createInstanceExternal(MonoObject* instance, MonoString* name, GUIElementStyle* externalStyle)
	{
		char* nativeName = mono_string_to_utf8(name);
		String styleName(nativeName);
		free(nativeName);

		ScriptGUIElementStyle* nativeInstance = new (bs_alloc<ScriptGUIElementStyle>()) ScriptGUIElementStyle(instance, styleName, externalStyle);
	}

	void ScriptGUIElementStyle::internal_addSubStyle(ScriptGUIElementStyle* nativeInstance, MonoString* guiType, MonoString* styleName)
	{
		String guiTypeStr = MonoUtil::monoToString(guiType);
		String styleNameStr = MonoUtil::monoToString(styleName);

		nativeInstance->getInternalValue()->subStyles[guiTypeStr] = styleNameStr;
	}
}