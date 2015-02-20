#include "BsScriptGUITextField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUITextField.h"
#include "BsGUIOptions.h"
#include "BsGUIContent.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUITextField::OnChangedThunkDef ScriptGUITextField::onChangedThunk;

	ScriptGUITextField::ScriptGUITextField(MonoObject* instance, GUITextField* textField)
		:TScriptGUIElement(instance, textField)
	{

	}

	void ScriptGUITextField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUITextField::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptGUITextField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", &ScriptGUITextField::internal_setValue);
		metaData.scriptClass->addInternalCall("Internal_HasInputFocus", &ScriptGUITextField::internal_hasInputFocus);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUITextField::internal_setTint);

		onChangedThunk = (OnChangedThunkDef)metaData.scriptClass->getMethod("DoOnChanged", 1)->getThunk();
	}

	void ScriptGUITextField::internal_createInstance(MonoObject* instance, bool multiline, MonoObject* title, UINT32 titleWidth,
		MonoString* style, MonoArray* guiOptions, bool withTitle)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		String styleName = toString(MonoUtil::monoToWString(style));

		GUITextField* guiField = nullptr;
		if (withTitle)
		{
			GUIContent nativeContent(ScriptGUIContent::getText(title), ScriptGUIContent::getImage(title), ScriptGUIContent::getTooltip(title));
			guiField = GUITextField::create(multiline, nativeContent, titleWidth, options, styleName);
		}
		else
		{
			guiField = GUITextField::create(multiline, options, styleName);
		}

		guiField->onValueChanged.connect(std::bind(&ScriptGUITextField::onChanged, instance, _1));

		ScriptGUITextField* nativeInstance = new (bs_alloc<ScriptGUITextField>()) ScriptGUITextField(instance, guiField);
	}

	void ScriptGUITextField::internal_getValue(ScriptGUITextField* nativeInstance, MonoString** output)
	{
		GUITextField* field = static_cast<GUITextField*>(nativeInstance->getGUIElement());
		*output = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), field->getValue());
	}

	void ScriptGUITextField::internal_setValue(ScriptGUITextField* nativeInstance, MonoString* value)
	{
		GUITextField* field = static_cast<GUITextField*>(nativeInstance->getGUIElement());
		return field->setValue(MonoUtil::monoToWString(value));
	}

	void ScriptGUITextField::internal_hasInputFocus(ScriptGUITextField* nativeInstance, bool* output)
	{
		GUITextField* field = static_cast<GUITextField*>(nativeInstance->getGUIElement());
		*output = field->hasInputFocus();
	}

	void ScriptGUITextField::internal_setTint(ScriptGUITextField* nativeInstance, Color color)
	{
		GUITextField* field = (GUITextField*)nativeInstance->getGUIElement();
		field->setTint(color);
	}

	void ScriptGUITextField::onChanged(MonoObject* instance, const WString& newValue)
	{
		MonoException* exception = nullptr;
		MonoString* monoNewValue = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), newValue);

		onChangedThunk(instance, monoNewValue, &exception);

		MonoUtil::throwIfException(exception);
	}
}