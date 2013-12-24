#include "BsScriptGUIElementStateStyle.h"
#include "BsScriptMeta.h"
#include "BsScriptField.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
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
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIElementStateStyle", &ScriptGUIElementStateStyle::initRuntimeData);

		ScriptManager::registerScriptType(&metaData);
	}

	void ScriptGUIElementStateStyle::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIElementStateStyle::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIElementStateStyle::internal_destroyInstance);

		metaData.scriptClass->addInternalCall("Internal_GetTexture", &ScriptGUIElementStateStyle::internal_GetTexture);
		metaData.scriptClass->addInternalCall("Internal_SetTexture", &ScriptGUIElementStateStyle::internal_SetTexture);

		metaData.scriptClass->addInternalCall("Internal_GetTextColor", &ScriptGUIElementStateStyle::internal_GetTextColor);
		metaData.scriptClass->addInternalCall("Internal_SetTextColor", &ScriptGUIElementStateStyle::internal_SetTextColor);
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
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	void ScriptGUIElementStateStyle::internal_GetTexture(ScriptGUIElementStateStyle* nativeInstance, MonoObject* value)
	{
		throwIfInstancesDontMatch(nativeInstance->mSpriteTexture, nativeInstance->mElementStateStyle->texture.get());

		if(nativeInstance->mSpriteTexture != nullptr)
		{
			value = nativeInstance->mSpriteTexture->getManagedInstance();
			return;
		}

		value = nullptr;
		return;
	}

	void ScriptGUIElementStateStyle::internal_SetTexture(ScriptGUIElementStateStyle* nativeInstance, ScriptSpriteTexture* value)
	{
		nativeInstance->mElementStateStyle->texture = value->getInternalTexture();
	}


	void ScriptGUIElementStateStyle::internal_GetTextColor(ScriptGUIElementStateStyle* nativeInstance, Color* value)
	{
		value = &nativeInstance->mElementStateStyle->textColor;
	}

	void ScriptGUIElementStateStyle::internal_SetTextColor(ScriptGUIElementStateStyle* nativeInstance, Color* value)
	{
		nativeInstance->mElementStateStyle->textColor = *value;
	}
}