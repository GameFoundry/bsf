#include "BsScriptGUIEnumField.h"
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
	ScriptGUIEnumField::OnSelectionChangedThunkDef ScriptGUIEnumField::onSelectionChangedThunk;

	ScriptGUIEnumField::ScriptGUIEnumField(MonoObject* instance, GUIListBoxField* listBoxField, const Vector<UINT32>& values)
		:TScriptGUIElement(instance, listBoxField), mValues(values)
	{

	}

	void ScriptGUIEnumField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIEnumField::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptGUIEnumField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", &ScriptGUIEnumField::internal_setValue);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUIEnumField::internal_setTint);

		onSelectionChangedThunk = (OnSelectionChangedThunkDef)metaData.scriptClass->getMethod("DoOnSelectionChanged", 1)->getThunk();
	}

	void ScriptGUIEnumField::internal_createInstance(MonoObject* instance, MonoArray* names, MonoArray* values, 
		MonoObject* title, UINT32 titleWidth, MonoString* style, MonoArray* guiOptions, bool withTitle)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		String styleName = toString(MonoUtil::monoToWString(style));

		ScriptArray namesArr(names);
		Vector<HString> nativeNames;
		for (UINT32 i = 0; i < namesArr.size(); i++)
			nativeNames.push_back(HString(namesArr.get<WString>(i)));

		GUIListBoxField* guiField = nullptr;
		if (withTitle)
		{
			GUIContent nativeContent(ScriptGUIContent::getText(title), ScriptGUIContent::getImage(title), ScriptGUIContent::getTooltip(title));
			guiField = GUIListBoxField::create(nativeNames, nativeContent, titleWidth, options, styleName);
		}
		else
		{
			guiField = GUIListBoxField::create(nativeNames, options, styleName);
		}

		guiField->onSelectionChanged.connect(std::bind(&ScriptGUIEnumField::onSelectionChanged, instance, _1));

		ScriptArray valuesArr(values);
		UINT32 elemSize = valuesArr.elementSize();

		Vector<UINT32> nativeValues;
		for (UINT32 i = 0; i < valuesArr.size(); i++)
		{
			// Truncate, as we don't check exact value type for enums
			UINT32 nativeValue = 0;

#if BS_ENDIAN == BS_ENDIAN_LITTLE
			memcpy(&nativeValue, valuesArr.getRawPtr(elemSize, i), std::min(elemSize, (UINT32)sizeof(UINT32)));
#else
			INT32 diff = (INT32)sizeof(UINT32) - (INT32)elemSize;
			UINT32 offsetDst = diff > 0 ? (UINT32)diff : 0;
			UINT32 offsetSrc = diff < 0 ? (UINT32)-diff : 0;

			memcpy(((UINT8*)&nativeValue) + offsetDst, valuesArr.getRawPtr(elemSize, i) + offsetSrc, 
				std::min(elemSize, (UINT32)sizeof(UINT32)));
#endif
		}
			

		ScriptGUIEnumField* nativeInstance = new (bs_alloc<ScriptGUIEnumField>()) ScriptGUIEnumField(instance, guiField, nativeValues);
	}

	UINT32 ScriptGUIEnumField::internal_getValue(ScriptGUIEnumField* nativeInstance)
	{
		GUIListBoxField* field = static_cast<GUIListBoxField*>(nativeInstance->getGUIElement());
		UINT32 idx = field->getIndex();

		const Vector<UINT32>& values = nativeInstance->mValues;
		if (idx < (UINT32)values.size())
			return values[idx];

		return 0;
	}

	void ScriptGUIEnumField::internal_setValue(ScriptGUIEnumField* nativeInstance, UINT32 value)
	{
		GUIListBoxField* field = static_cast<GUIListBoxField*>(nativeInstance->getGUIElement());

		const Vector<UINT32>& values = nativeInstance->mValues;
		for (UINT32 i = 0; i < (UINT32)values.size(); i++)
		{
			if (values[i] == value)
			{
				field->setIndex(i);
				break;
			}
		}
	}

	void ScriptGUIEnumField::internal_setTint(ScriptGUIEnumField* nativeInstance, Color color)
	{
		GUIListBoxField* field = (GUIListBoxField*)nativeInstance->getGUIElement();
		field->setTint(color);
	}

	void ScriptGUIEnumField::onSelectionChanged(MonoObject* instance, UINT32 newIndex)
	{
		MonoUtil::invokeThunk(onSelectionChangedThunk, instance, newIndex);
	}
}