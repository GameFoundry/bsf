#include "BsScriptGUIInputBox.h"
#include "BsScriptMeta.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsGUIInputBox.h"
#include "BsGUIOptions.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIInputBox::OnChangedThunkDef ScriptGUIInputBox::onChangedThunk;
	ScriptGUIInputBox::OnConfirmedThunkDef ScriptGUIInputBox::onConfirmedThunk;

	ScriptGUIInputBox::ScriptGUIInputBox(MonoObject* instance, GUIInputBox* inputBox)
		:TScriptGUIElement(instance, inputBox)
	{

	}

	void ScriptGUIInputBox::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIInputBox::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetText", &ScriptGUIInputBox::internal_getText);
		metaData.scriptClass->addInternalCall("Internal_SetText", &ScriptGUIInputBox::internal_setText);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUIInputBox::internal_setTint);

		onChangedThunk = (OnChangedThunkDef)metaData.scriptClass->getMethod("Internal_DoOnChanged", 1)->getThunk();
		onConfirmedThunk = (OnConfirmedThunkDef)metaData.scriptClass->getMethod("Internal_DoOnConfirmed", 0)->getThunk();
	}

	void ScriptGUIInputBox::internal_createInstance(MonoObject* instance, bool multiline, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIInputBox* guiInputBox = GUIInputBox::create(multiline, options, toString(MonoUtil::monoToWString(style)));
		guiInputBox->onValueChanged.connect(std::bind(&ScriptGUIInputBox::onChanged, instance, _1));

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

	void ScriptGUIInputBox::internal_setTint(ScriptGUIInputBox* nativeInstance, Color color)
	{
		GUIInputBox* inputBox = (GUIInputBox*)nativeInstance->getGUIElement();
		inputBox->setTint(color);
	}

	void ScriptGUIInputBox::onChanged(MonoObject* instance, const WString& newValue)
	{
		MonoString* monoValue = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), newValue);
		MonoUtil::invokeThunk(onChangedThunk, instance, monoValue);
	}

	void ScriptGUIInputBox::onConfirmed(MonoObject* instance)
	{
		MonoUtil::invokeThunk(onConfirmedThunk, instance);
	}
}