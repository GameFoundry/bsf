#include "BsScriptGUIElementStateStyle.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptSpriteTexture.h"
#include "CmException.h"
#include "BsGUIElementStyle.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIElementStateStyle::ScriptGUIElementStateStyle()
		:mElementStateStyle(cm_new<GUIElementStyle::GUIElementStateStyle>()), mSpriteTexture(nullptr), mOwnsStyle(true)
	{

	}

	ScriptGUIElementStateStyle::ScriptGUIElementStateStyle(GUIElementStyle::GUIElementStateStyle* externalStyle)
		: mElementStateStyle(externalStyle), mSpriteTexture(nullptr), mOwnsStyle(false)
	{

	}

	ScriptGUIElementStateStyle::~ScriptGUIElementStateStyle()
	{
		if(mOwnsStyle)
			cm_delete(mElementStateStyle);
	}

	void ScriptGUIElementStateStyle::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUIElementStateStyle", &ScriptGUIElementStateStyle::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIElementStateStyle::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIElementStateStyle::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIElementStateStyle::internal_destroyInstance);

		CM_SCRIPT_SETGET_META(ScriptGUIElementStateStyle, Texture);
		CM_SCRIPT_SETGET_META(ScriptGUIElementStateStyle, TextColor);
	}

	void ScriptGUIElementStateStyle::internal_createInstance(MonoObject* instance)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle();
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIElementStateStyle::internal_createInstanceExternal(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStyle)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle(externalStyle);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIElementStateStyle::internal_destroyInstance(ScriptGUIElementStateStyle* nativeInstance)
	{
		cm_delete(nativeInstance);
	}
}