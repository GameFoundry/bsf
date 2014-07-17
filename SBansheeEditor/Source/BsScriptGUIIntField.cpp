#include "BsScriptGUIIntField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIIntField.h"
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
	ScriptGUIIntField::OnChangedThunkDef ScriptGUIIntField::onChangedThunk;

	ScriptGUIIntField::ScriptGUIIntField(MonoObject* instance, GUIIntField* intField)
		:TScriptGUIElement(instance, intField)
	{

	}

	void ScriptGUIIntField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIIntField::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptGUIIntField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", &ScriptGUIIntField::internal_setValue);
		metaData.scriptClass->addInternalCall("Internal_HasInputFocus", &ScriptGUIIntField::internal_hasInputFocus);

		onChangedThunk = (OnChangedThunkDef)metaData.scriptClass->getMethod("DoOnChanged", 1).getThunk();
	}

	void ScriptGUIIntField::internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth, 
		MonoString* style, MonoArray* guiOptions, bool withTitle)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		String styleName = toString(MonoUtil::monoToWString(style));

		GUIIntField* guiIntField = nullptr;
		if(withTitle)
		{
			GUIContent nativeContent(ScriptGUIContent::getText(title), ScriptGUIContent::getImage(title), ScriptGUIContent::getTooltip(title));
			guiIntField = GUIIntField::create(nativeContent, titleWidth, options, styleName);
		}
		else
		{
			guiIntField = GUIIntField::create(options, styleName);
		}

		guiIntField->onValueChanged.connect(std::bind(&ScriptGUIIntField::onChanged, instance, _1));

		ScriptGUIIntField* nativeInstance = new (bs_alloc<ScriptGUIIntField>()) ScriptGUIIntField(instance, guiIntField);
	}

	void ScriptGUIIntField::internal_getValue(ScriptGUIIntField* nativeInstance, INT32* output)
	{
		GUIIntField* intField = static_cast<GUIIntField*>(nativeInstance->getGUIElement());

		*output = intField->getValue();
	}

	void ScriptGUIIntField::internal_setValue(ScriptGUIIntField* nativeInstance, INT32 value)
	{
		GUIIntField* intField = static_cast<GUIIntField*>(nativeInstance->getGUIElement());

		return intField->setValue(value);
	}

	void ScriptGUIIntField::internal_hasInputFocus(ScriptGUIIntField* nativeInstance, bool* output)
	{
		GUIIntField* intField = static_cast<GUIIntField*>(nativeInstance->getGUIElement());

		*output = intField->hasInputFocus();
	}

	void ScriptGUIIntField::onChanged(MonoObject* instance, INT32 newValue)
	{
		MonoException* exception = nullptr;
		onChangedThunk(instance, newValue, &exception);

		MonoUtil::throwIfException(exception);
	}
}