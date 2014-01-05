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

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIListBox::OnSelectionChangedThunkDef ScriptGUIListBox::onSelectionChangedThunk;

	ScriptGUIListBox::ScriptGUIListBox(GUIListBox* listBox)
		:mListBox(listBox)
	{

	}

	void ScriptGUIListBox::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIListBox", &ScriptGUIListBox::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIListBox::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIListBox::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIListBox::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_SetElements", &ScriptGUIListBox::internal_setElements);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIListBox::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_Enable", &ScriptGUIListBox::internal_enable);
		metaData.scriptClass->addInternalCall("Internal_Disable", &ScriptGUIListBox::internal_disable);

		onSelectionChangedThunk = (OnSelectionChangedThunkDef)metaData.scriptClass->getMethod("DoOnSelectionChanged", 1).getThunk();
	}

	void ScriptGUIListBox::internal_createInstance(MonoObject* instance, MonoObject* parentLayout, MonoArray* elements, MonoObject* style, MonoArray* guiOptions)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUIOptions options;

		UINT32 optionsArrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < optionsArrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIElementStyle* elemStyle = nullptr;

		if(style != nullptr)
			elemStyle = ScriptGUIElementStyle::toNative(style)->getInternalValue();

		UINT32 elementsArrayLen = (UINT32)mono_array_length(elements);
		Vector<HString>::type nativeElements;
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

		GUIListBox* guiListBox = GUIListBox::create(scriptLayout->getParentWidget(), nativeElements, options, elemStyle);
		guiListBox->onSelectionChanged.connect(std::bind(&ScriptGUIListBox::onSelectionChanged, instance, std::placeholders::_1));

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(guiListBox);

		ScriptGUIListBox* nativeInstance = new (cm_alloc<ScriptGUIListBox>()) ScriptGUIListBox(guiListBox);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIListBox::internal_destroyInstance(ScriptGUIListBox* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	void ScriptGUIListBox::internal_setElements(ScriptGUIListBox* nativeInstance, MonoArray* elements)
	{
		UINT32 elementsArrayLen = (UINT32)mono_array_length(elements);
		Vector<HString>::type nativeElements;
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

		nativeInstance->getInternalValue()->setElements(nativeElements);
	}

	void ScriptGUIListBox::internal_destroy(ScriptGUIListBox* nativeInstance)
	{
		GUIElement::destroy(nativeInstance->getInternalValue());
	}

	void ScriptGUIListBox::internal_disable(ScriptGUIListBox* nativeInstance)
	{
		nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUIListBox::internal_enable(ScriptGUIListBox* nativeInstance)
	{
		nativeInstance->getInternalValue()->enableRecursively();
	}

	void ScriptGUIListBox::onSelectionChanged(MonoObject* instance, CM::UINT32 index)
	{
		MonoException* exception = nullptr;
		onSelectionChangedThunk(instance, index, &exception);

		MonoUtil::throwIfException(exception);
	}
}