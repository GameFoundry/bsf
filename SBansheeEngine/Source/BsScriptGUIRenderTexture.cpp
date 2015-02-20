#include "BsScriptGUIRenderTexture.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIRenderTexture.h"
#include "BsGUIOptions.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"
#include "BsScriptRenderTexture2D.h"

namespace BansheeEngine
{
	ScriptGUIRenderTexture::ScriptGUIRenderTexture(MonoObject* instance, GUIRenderTexture* texture)
		:TScriptGUIElement(instance, texture)
	{

	}

	void ScriptGUIRenderTexture::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIRenderTexture::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetTexture", &ScriptGUIRenderTexture::internal_setTexture);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUIRenderTexture::internal_setTint);
	}

	void ScriptGUIRenderTexture::internal_createInstance(MonoObject* instance, 
		ScriptRenderTexture2D* texture, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		RenderTexturePtr renderTexture;
		if (texture != nullptr)
			renderTexture = texture->getRenderTexture();

		GUIRenderTexture* guiTexture = GUIRenderTexture::create(renderTexture, options, toString(MonoUtil::monoToWString(style)));

		ScriptGUIRenderTexture* nativeInstance = new (bs_alloc<ScriptGUIRenderTexture>()) ScriptGUIRenderTexture(instance, guiTexture);
	}

	void ScriptGUIRenderTexture::internal_setTexture(ScriptGUIRenderTexture* nativeInstance, ScriptRenderTexture2D* texture)
	{
		RenderTexturePtr renderTexture;
		if (texture != nullptr)
			renderTexture = texture->getRenderTexture();

		GUIRenderTexture* guiTexture = (GUIRenderTexture*)nativeInstance->getGUIElement();
		guiTexture->setRenderTexture(renderTexture);
	}

	void ScriptGUIRenderTexture::internal_setTint(ScriptGUIRenderTexture* nativeInstance, Color color)
	{
		GUIRenderTexture* guiTexture = (GUIRenderTexture*)nativeInstance->getGUIElement();
		guiTexture->setTint(color);
	}
}