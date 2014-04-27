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
		:ScriptObject(instance), mElementStateStyle(cm_new<GUIElementStyle::GUIElementStateStyle>()), mSpriteTexture(nullptr), mOwnsStyle(true)
	{

	}

	ScriptGUIElementStateStyle::ScriptGUIElementStateStyle(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStyle)
		:ScriptObject(instance), mElementStateStyle(externalStyle), mSpriteTexture(nullptr), mOwnsStyle(false)
	{

	}

	ScriptGUIElementStateStyle::~ScriptGUIElementStateStyle()
	{
		if(mOwnsStyle)
			cm_delete(mElementStateStyle);
	}

	void ScriptGUIElementStateStyle::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIElementStateStyle::internal_createInstance);

		CM_SCRIPT_SETGET_META(ScriptGUIElementStateStyle, Texture);
		CM_SCRIPT_SETGET_META(ScriptGUIElementStateStyle, TextColor);
	}

	void ScriptGUIElementStateStyle::internal_createInstance(MonoObject* instance)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle(instance);
	}

	void ScriptGUIElementStateStyle::internal_createInstanceExternal(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStyle)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle(instance, externalStyle);
	}
}