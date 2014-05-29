#include "BsScriptGUITexture.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIOptions.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

namespace BansheeEngine
{
	ScriptGUITexture::ScriptGUITexture(MonoObject* instance, GUITexture* texture)
		:TScriptGUIElement(instance, texture)
	{

	}

	void ScriptGUITexture::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUITexture::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetTexture", &ScriptGUITexture::internal_setTexture);
	}

	void ScriptGUITexture::internal_createInstance(MonoObject* instance, MonoObject* texture, 
		GUIImageScaleMode scale, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		HSpriteTexture nativeTexture;
		if(texture != nullptr)
			nativeTexture = ScriptSpriteTexture::toNative(texture)->getInternalValue();

		GUITexture* guiTexture = GUITexture::create(nativeTexture, scale, options, toString(MonoUtil::monoToWString(style)));

		ScriptGUITexture* nativeInstance = new (bs_alloc<ScriptGUITexture>()) ScriptGUITexture(instance, guiTexture);
	}

	void ScriptGUITexture::internal_setTexture(ScriptGUITexture* nativeInstance, MonoObject* texture)
	{
		HSpriteTexture nativeTexture;
		if(texture != nullptr)
			nativeTexture = ScriptSpriteTexture::toNative(texture)->getInternalValue();

		GUITexture* guiTexture = (GUITexture*)nativeInstance->getGUIElement();
		guiTexture->setTexture(nativeTexture);
	}
}