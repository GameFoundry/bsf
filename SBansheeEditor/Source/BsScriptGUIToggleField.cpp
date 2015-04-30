#include "BsScriptGUIToggleField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIToggleField.h"
#include "BsGUIOptions.h"
#include "BsGUIContent.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIToggleField::OnChangedThunkDef ScriptGUIToggleField::onChangedThunk;

	ScriptGUIToggleField::ScriptGUIToggleField(MonoObject* instance, GUIToggleField* toggleField)
		:TScriptGUIElement(instance, toggleField)
	{

	}

	void ScriptGUIToggleField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIToggleField::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptGUIToggleField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", &ScriptGUIToggleField::internal_setValue);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUIToggleField::internal_setTint);

		onChangedThunk = (OnChangedThunkDef)metaData.scriptClass->getMethod("DoOnChanged", 1)->getThunk();
	}

	void ScriptGUIToggleField::internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
		MonoString* style, MonoArray* guiOptions, bool withTitle)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		String styleName = toString(MonoUtil::monoToWString(style));

		GUIToggleField* guiField = nullptr;
		if (withTitle)
		{
			GUIContent nativeContent(ScriptGUIContent::getText(title), ScriptGUIContent::getImage(title), ScriptGUIContent::getTooltip(title));
			guiField = GUIToggleField::create(nativeContent, titleWidth, options, styleName);
		}
		else
		{
			guiField = GUIToggleField::create(options, styleName);
		}

		guiField->onValueChanged.connect(std::bind(&ScriptGUIToggleField::onChanged, instance, _1));

		ScriptGUIToggleField* nativeInstance = new (bs_alloc<ScriptGUIToggleField>()) ScriptGUIToggleField(instance, guiField);
	}

	void ScriptGUIToggleField::internal_getValue(ScriptGUIToggleField* nativeInstance, bool* output)
	{
		GUIToggleField* toggleField = static_cast<GUIToggleField*>(nativeInstance->getGUIElement());
		*output = toggleField->getValue();
	}

	void ScriptGUIToggleField::internal_setValue(ScriptGUIToggleField* nativeInstance, bool value)
	{
		GUIToggleField* toggleField = static_cast<GUIToggleField*>(nativeInstance->getGUIElement());
		return toggleField->setValue(value);
	}

	void ScriptGUIToggleField::internal_setTint(ScriptGUIToggleField* nativeInstance, Color color)
	{
		GUIToggleField* toggleField = (GUIToggleField*)nativeInstance->getGUIElement();
		toggleField->setTint(color);
	}

	void ScriptGUIToggleField::onChanged(MonoObject* instance, bool newValue)
	{
		MonoException* exception = nullptr;
		onChangedThunk(instance, newValue, &exception);

		MonoUtil::throwIfException(exception);
	}
}