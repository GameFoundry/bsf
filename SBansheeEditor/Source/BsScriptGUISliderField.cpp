#include "BsScriptGUISliderField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsGUISliderField.h"
#include "BsGUIOptions.h"
#include "BsGUIContent.h"
#include "BsScriptGUIContent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUISliderField::OnChangedThunkDef ScriptGUISliderField::onChangedThunk;

	ScriptGUISliderField::ScriptGUISliderField(MonoObject* instance, GUISliderField* sliderField)
		:TScriptGUIElement(instance, sliderField)
	{

	}

	void ScriptGUISliderField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUISliderField::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptGUISliderField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", &ScriptGUISliderField::internal_setValue);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUISliderField::internal_setTint);
		metaData.scriptClass->addInternalCall("Internal_SetRange", &ScriptGUISliderField::internal_setRange);
		metaData.scriptClass->addInternalCall("Internal_SetStep", &ScriptGUISliderField::internal_setStep);

		onChangedThunk = (OnChangedThunkDef)metaData.scriptClass->getMethod("DoOnChanged", 1)->getThunk();
	}

	void ScriptGUISliderField::internal_createInstance(MonoObject* instance, float min, float max, MonoObject* title, UINT32 titleWidth,
		MonoString* style, MonoArray* guiOptions, bool withTitle)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		String styleName = toString(MonoUtil::monoToWString(style));

		GUISliderField* guiSliderField = nullptr;
		if (withTitle)
		{
			GUIContent nativeContent(ScriptGUIContent::getText(title), ScriptGUIContent::getImage(title), ScriptGUIContent::getTooltip(title));
			guiSliderField = GUISliderField::create(nativeContent, titleWidth, options, styleName);
		}
		else
		{
			guiSliderField = GUISliderField::create(options, styleName);
		}

		guiSliderField->setRange(min, max);
		guiSliderField->onValueChanged.connect(std::bind(&ScriptGUISliderField::onChanged, instance, _1));

		ScriptGUISliderField* nativeInstance = new (bs_alloc<ScriptGUISliderField>()) ScriptGUISliderField(instance, guiSliderField);
	}

	float ScriptGUISliderField::internal_getValue(ScriptGUISliderField* nativeInstance)
	{
		GUISliderField* sliderField = static_cast<GUISliderField*>(nativeInstance->getGUIElement());
		return sliderField->getValue();
	}

	void ScriptGUISliderField::internal_setValue(ScriptGUISliderField* nativeInstance, float value)
	{
		GUISliderField* sliderField = static_cast<GUISliderField*>(nativeInstance->getGUIElement());
		return sliderField->setValue(value);
	}

	void ScriptGUISliderField::internal_setTint(ScriptGUISliderField* nativeInstance, Color color)
	{
		GUISliderField* sliderField = (GUISliderField*)nativeInstance->getGUIElement();
		sliderField->setTint(color);
	}

	void ScriptGUISliderField::internal_setRange(ScriptGUISliderField* nativeInstance, float min, float max)
	{
		GUISliderField* sliderField = static_cast<GUISliderField*>(nativeInstance->getGUIElement());
		sliderField->setRange(min, max);
	}

	void ScriptGUISliderField::internal_setStep(ScriptGUISliderField* nativeInstance, float step)
	{
		GUISliderField* sliderField = static_cast<GUISliderField*>(nativeInstance->getGUIElement());
		sliderField->setStep(step);
	}

	void ScriptGUISliderField::onChanged(MonoObject* instance, float newValue)
	{
		MonoUtil::invokeThunk(onChangedThunk, instance, newValue);
	}
}