#include "BsScriptGUIListBoxField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsGUIListBoxField.h"
#include "BsGUIOptions.h"
#include "BsGUIContent.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIListBoxField::OnSelectionChangedThunkDef ScriptGUIListBoxField::onSelectionChangedThunk;

	ScriptGUIListBoxField::ScriptGUIListBoxField(MonoObject* instance, GUIListBoxField* listBoxField)
		:TScriptGUIElement(instance, listBoxField)
	{

	}

	void ScriptGUIListBoxField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIListBoxField::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetElements", &ScriptGUIListBoxField::internal_setElements);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptGUIListBoxField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", &ScriptGUIListBoxField::internal_setValue);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUIListBoxField::internal_setTint);

		onSelectionChangedThunk = (OnSelectionChangedThunkDef)metaData.scriptClass->getMethod("DoOnSelectionChanged", 1)->getThunk();
	}

	void ScriptGUIListBoxField::internal_createInstance(MonoObject* instance, MonoArray* elements, MonoObject* title, 
		UINT32 titleWidth, MonoString* style, MonoArray* guiOptions, bool withTitle)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		String styleName = toString(MonoUtil::monoToWString(style));

		UINT32 elementsArrayLen = (UINT32)mono_array_length(elements);
		Vector<HString> nativeElements;
		for (UINT32 i = 0; i < elementsArrayLen; i++)
		{
			MonoObject* stringManaged = (MonoObject*)mono_array_get(elements, MonoObject*, i);

			if (stringManaged == nullptr)
				nativeElements.push_back(HString::dummy());
			else
			{
				ScriptHString* textScript = ScriptHString::toNative(stringManaged);
				nativeElements.push_back(textScript->getInternalValue());
			}
		}

		GUIListBoxField* guiField = nullptr;
		if (withTitle)
		{
			GUIContent nativeContent(ScriptGUIContent::getText(title), ScriptGUIContent::getImage(title), ScriptGUIContent::getTooltip(title));
			guiField = GUIListBoxField::create(nativeElements, nativeContent, titleWidth, options, styleName);
		}
		else
		{
			guiField = GUIListBoxField::create(nativeElements, options, styleName);
		}

		guiField->onSelectionChanged.connect(std::bind(&ScriptGUIListBoxField::onSelectionChanged, instance, _1));

		ScriptGUIListBoxField* nativeInstance = new (bs_alloc<ScriptGUIListBoxField>()) ScriptGUIListBoxField(instance, guiField);
	}

	UINT32 ScriptGUIListBoxField::internal_getValue(ScriptGUIListBoxField* nativeInstance)
	{
		GUIListBoxField* field = static_cast<GUIListBoxField*>(nativeInstance->getGUIElement());
		return field->getIndex();
	}

	void ScriptGUIListBoxField::internal_setValue(ScriptGUIListBoxField* nativeInstance, UINT32 index)
	{
		GUIListBoxField* field = static_cast<GUIListBoxField*>(nativeInstance->getGUIElement());
		return field->setIndex(index);
	}

	void ScriptGUIListBoxField::internal_setElements(ScriptGUIListBoxField* nativeInstance, MonoArray* elements)
	{
		UINT32 elementsArrayLen = (UINT32)mono_array_length(elements);
		Vector<HString> nativeElements;
		for (UINT32 i = 0; i < elementsArrayLen; i++)
		{
			MonoObject* stringManaged = (MonoObject*)mono_array_get(elements, MonoObject*, i);

			if (stringManaged == nullptr)
				nativeElements.push_back(HString::dummy());
			else
			{
				ScriptHString* textScript = ScriptHString::toNative(stringManaged);
				nativeElements.push_back(textScript->getInternalValue());
			}
		}

		GUIListBoxField* field = static_cast<GUIListBoxField*>(nativeInstance->getGUIElement());
		field->setElements(nativeElements);
	}

	void ScriptGUIListBoxField::internal_setTint(ScriptGUIListBoxField* nativeInstance, Color color)
	{
		GUIListBoxField* field = (GUIListBoxField*)nativeInstance->getGUIElement();
		field->setTint(color);
	}

	void ScriptGUIListBoxField::onSelectionChanged(MonoObject* instance, UINT32 newIndex)
	{
		MonoUtil::invokeThunk(onSelectionChangedThunk, instance, newIndex);
	}
}