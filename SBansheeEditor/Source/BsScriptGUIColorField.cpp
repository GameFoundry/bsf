#include "BsScriptGUIColorField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIColorField.h"
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
	ScriptGUIColorField::OnChangedThunkDef ScriptGUIColorField::onChangedThunk;

	ScriptGUIColorField::ScriptGUIColorField(MonoObject* instance, GUIColorField* colorField)
		:TScriptGUIElement(instance, colorField)
	{

	}

	void ScriptGUIColorField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIColorField::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptGUIColorField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", &ScriptGUIColorField::internal_setValue);

		onChangedThunk = (OnChangedThunkDef)metaData.scriptClass->getMethod("DoOnChanged", 1).getThunk();
	}

	void ScriptGUIColorField::internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
		MonoString* style, MonoArray* guiOptions, bool withTitle)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		String styleName = toString(MonoUtil::monoToWString(style));

		GUIColorField* guiField = nullptr;
		if (withTitle)
		{
			GUIContent nativeContent(ScriptGUIContent::getText(title), ScriptGUIContent::getImage(title), ScriptGUIContent::getTooltip(title));
			guiField = GUIColorField::create(nativeContent, titleWidth, options, styleName);
		}
		else
		{
			guiField = GUIColorField::create(options, styleName);
		}

		guiField->onValueChanged.connect(std::bind(&ScriptGUIColorField::onChanged, instance, _1));

		ScriptGUIColorField* nativeInstance = new (bs_alloc<ScriptGUIColorField>()) ScriptGUIColorField(instance, guiField);
	}

	void ScriptGUIColorField::internal_getValue(ScriptGUIColorField* nativeInstance, Color* output)
	{
		GUIColorField* colorField = static_cast<GUIColorField*>(nativeInstance->getGUIElement());

		*output = colorField->getValue();
	}

	void ScriptGUIColorField::internal_setValue(ScriptGUIColorField* nativeInstance, Color value)
	{
		GUIColorField* colorField = static_cast<GUIColorField*>(nativeInstance->getGUIElement());

		return colorField->setValue(value);
	}

	void ScriptGUIColorField::onChanged(MonoObject* instance, Color newValue)
	{
		MonoException* exception = nullptr;
		onChangedThunk(instance, newValue, &exception);

		MonoUtil::throwIfException(exception);
	}
}