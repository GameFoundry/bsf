#include "BsScriptGUIListBox.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIListBox.h"
#include "BsGUIOptions.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIListBox::OnSelectionChangedThunkDef ScriptGUIListBox::onSelectionChangedThunk;

	ScriptGUIListBox::ScriptGUIListBox(MonoObject* instance, GUIListBox* listBox)
		:TScriptGUIElement(instance, listBox)
	{

	}

	void ScriptGUIListBox::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIListBox::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetElements", &ScriptGUIListBox::internal_setElements);

		onSelectionChangedThunk = (OnSelectionChangedThunkDef)metaData.scriptClass->getMethod("DoOnSelectionChanged", 1).getThunk();
	}

	void ScriptGUIListBox::internal_createInstance(MonoObject* instance, MonoArray* elements, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 optionsArrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < optionsArrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		UINT32 elementsArrayLen = (UINT32)mono_array_length(elements);
		Vector<HString> nativeElements;
		for(UINT32 i = 0; i < elementsArrayLen; i++)
		{
			MonoObject* stringManaged = (MonoObject*)mono_array_get(elements, MonoObject*, i);

			if(stringManaged == nullptr)
				nativeElements.push_back(HString::dummy());
			else
			{
				ScriptHString* textScript = ScriptHString::toNative(stringManaged);
				nativeElements.push_back(textScript->getInternalValue());
			}
		}

		GUIListBox* guiListBox = GUIListBox::create(nativeElements, options, toString(MonoUtil::monoToWString(style)));
		guiListBox->onSelectionChanged.connect(std::bind(&ScriptGUIListBox::onSelectionChanged, instance, std::placeholders::_1));

		ScriptGUIListBox* nativeInstance = new (cm_alloc<ScriptGUIListBox>()) ScriptGUIListBox(instance, guiListBox);
	}

	void ScriptGUIListBox::internal_setElements(ScriptGUIListBox* nativeInstance, MonoArray* elements)
	{
		UINT32 elementsArrayLen = (UINT32)mono_array_length(elements);
		Vector<HString> nativeElements;
		for(UINT32 i = 0; i < elementsArrayLen; i++)
		{
			MonoObject* stringManaged = (MonoObject*)mono_array_get(elements, MonoObject*, i);

			if(stringManaged == nullptr)
				nativeElements.push_back(HString::dummy());
			else
			{
				ScriptHString* textScript = ScriptHString::toNative(stringManaged);
				nativeElements.push_back(textScript->getInternalValue());
			}
		}

		GUIListBox* listBox = (GUIListBox*)nativeInstance->getGUIElement();
		listBox->setElements(nativeElements);
	}

	void ScriptGUIListBox::onSelectionChanged(MonoObject* instance, UINT32 index)
	{
		MonoException* exception = nullptr;
		onSelectionChangedThunk(instance, index, &exception);

		MonoUtil::throwIfException(exception);
	}
}