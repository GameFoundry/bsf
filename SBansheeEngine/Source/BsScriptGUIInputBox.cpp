#include "BsScriptGUIInputBox.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIInputBox.h"
#include "BsGUIOptions.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

namespace BansheeEngine
{
	ScriptGUIInputBox::ScriptGUIInputBox(MonoObject* instance, GUIInputBox* inputBox)
		:TScriptGUIElement(instance, inputBox)
	{

	}

	void ScriptGUIInputBox::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIInputBox::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetText", &ScriptGUIInputBox::internal_getText);
		metaData.scriptClass->addInternalCall("Internal_SetText", &ScriptGUIInputBox::internal_setText);
	}

	void ScriptGUIInputBox::internal_createInstance(MonoObject* instance, bool multiline, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIInputBox* guiInputBox = GUIInputBox::create(multiline, options, toString(MonoUtil::monoToWString(style)));

		ScriptGUIInputBox* nativeInstance = new (bs_alloc<ScriptGUIInputBox>()) ScriptGUIInputBox(instance, guiInputBox);
	}

	void ScriptGUIInputBox::internal_getText(ScriptGUIInputBox* nativeInstance, MonoString** text)
	{
		GUIInputBox* inputBox = (GUIInputBox*)nativeInstance->getGUIElement();

		*text = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), inputBox->getText());
	}

	void ScriptGUIInputBox::internal_setText(ScriptGUIInputBox* nativeInstance, MonoString* text)
	{
		GUIInputBox* inputBox = (GUIInputBox*)nativeInstance->getGUIElement();

		inputBox->setText(MonoUtil::monoToWString(text));
	}
}