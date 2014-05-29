#include "BsScriptGUIElementStateStyle.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptSpriteTexture.h"
#include "CmException.h"
#include "BsGUIElementStyle.h"

namespace BansheeEngine
{
	ScriptGUIElementStateStyle::ScriptGUIElementStateStyle(MonoObject* instance)
		:ScriptObject(instance), mElementStateStyle(bs_new<GUIElementStyle::GUIElementStateStyle>()), mSpriteTexture(nullptr), mOwnsStyle(true)
	{

	}

	ScriptGUIElementStateStyle::ScriptGUIElementStateStyle(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStyle)
		:ScriptObject(instance), mElementStateStyle(externalStyle), mSpriteTexture(nullptr), mOwnsStyle(false)
	{

	}

	ScriptGUIElementStateStyle::~ScriptGUIElementStateStyle()
	{
		if(mOwnsStyle)
			bs_delete(mElementStateStyle);
	}

	void ScriptGUIElementStateStyle::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIElementStateStyle::internal_createInstance);

		BS_SCRIPT_SETGET_META(ScriptGUIElementStateStyle, Texture);
		BS_SCRIPT_SETGET_META(ScriptGUIElementStateStyle, TextColor);
	}

	void ScriptGUIElementStateStyle::internal_createInstance(MonoObject* instance)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (bs_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle(instance);
	}

	void ScriptGUIElementStateStyle::internal_createInstanceExternal(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStyle)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (bs_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle(instance, externalStyle);
	}
}