#include "BsScriptGUIElementStateStyle.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptSpriteTexture.h"
#include "BsException.h"
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
		metaData.scriptClass->addInternalCall("Internal_GetTexture", &ScriptGUIElementStateStyle::internal_GetTexture);	
		metaData.scriptClass->addInternalCall("Internal_SetTexture", &ScriptGUIElementStateStyle::internal_SetTexture);
		metaData.scriptClass->addInternalCall("Internal_GetTextColor", &ScriptGUIElementStateStyle::internal_GetTextColor);
		metaData.scriptClass->addInternalCall("Internal_SetTextColor", &ScriptGUIElementStateStyle::internal_SetTextColor);
	}

	void ScriptGUIElementStateStyle::internal_createInstance(MonoObject* instance)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (bs_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle(instance);
	}

	void ScriptGUIElementStateStyle::internal_createInstanceExternal(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStyle)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (bs_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle(instance, externalStyle);
	}

	void ScriptGUIElementStateStyle::internal_GetTexture(ScriptGUIElementStateStyle* nativeInstance, MonoObject** value)
	{
		if (nativeInstance->mSpriteTexture != nullptr)
		{	
			*value = nativeInstance->mSpriteTexture->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStateStyle::internal_SetTexture(ScriptGUIElementStateStyle* nativeInstance, MonoObject* value)
	{
		ScriptSpriteTexture* nativeValue = ScriptSpriteTexture::toNative(value);
		nativeInstance->mElementStateStyle->texture = nativeValue->getHandle();
		nativeInstance->mSpriteTexture = nativeValue;
	}

	void ScriptGUIElementStateStyle::internal_GetTextColor(ScriptGUIElementStateStyle* nativeInstance, Color* value)
	{
		*value = nativeInstance->mElementStateStyle->textColor;
	}

	void ScriptGUIElementStateStyle::internal_SetTextColor(ScriptGUIElementStateStyle* nativeInstance, Color* value)
	{
		nativeInstance->mElementStateStyle->textColor = *value;
	}	
}