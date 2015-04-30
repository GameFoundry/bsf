#include "BsScriptGUILabel.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIOptions.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

namespace BansheeEngine
{
	ScriptGUILabel::ScriptGUILabel(MonoObject* instance, GUILabel* label)
		:TScriptGUIElement(instance, label)
	{

	}

	void ScriptGUILabel::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUILabel::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUILabel::internal_setContent);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUILabel::internal_setTint);
	}

	void ScriptGUILabel::internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		GUILabel* guiLabel = GUILabel::create(nativeContent, options, toString(MonoUtil::monoToWString(style)));

		ScriptGUILabel* nativeInstance = new (bs_alloc<ScriptGUILabel>()) ScriptGUILabel(instance, guiLabel);
	}

	void ScriptGUILabel::internal_setContent(ScriptGUILabel* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));

		GUILabel* label = (GUILabel*)nativeInstance->getGUIElement();
		label->setContent(nativeContent);
	}

	void ScriptGUILabel::internal_setTint(ScriptGUILabel* nativeInstance, Color color)
	{
		GUILabel* label = (GUILabel*)nativeInstance->getGUIElement();
		label->setTint(color);
	}
}