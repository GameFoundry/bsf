#include "BsScriptGUIElementStyle.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptFont.h"
#include "CmException.h"
#include "BsGUIElementStyle.h"
#include "BsScriptGUIElementStateStyle.h"

namespace BansheeEngine
{
	ScriptGUIElementStyle::ScriptGUIElementStyle(MonoObject* instance, const String& name)
		:ScriptObject(instance), mName(name), mElementStyle(cm_new<GUIElementStyle>()), mFont(nullptr), mOwnsStyle(true), mNormal(nullptr), mHover(nullptr),
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
			cm_delete(mElementStyle);
	}

	void ScriptGUIElementStyle::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIElementStyle::internal_createInstance);

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

	void ScriptGUIElementStyle::internal_createInstance(MonoObject* instance, MonoString* name)
	{
		char* nativeName = mono_string_to_utf8(name);
		String styleName(nativeName);
		free(nativeName);

		ScriptGUIElementStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStyle>()) ScriptGUIElementStyle(instance, styleName);
	}

	void ScriptGUIElementStyle::internal_createInstanceExternal(MonoObject* instance, MonoString* name, GUIElementStyle* externalStyle)
	{
		char* nativeName = mono_string_to_utf8(name);
		String styleName(nativeName);
		free(nativeName);

		ScriptGUIElementStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStyle>()) ScriptGUIElementStyle(instance, styleName, externalStyle);
	}
}