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
using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIListBox::OnSelectionChangedThunkDef ScriptGUIListBox::onSelectionChangedThunk;

	ScriptGUIListBox::ScriptGUIListBox(GUIListBox* listBox)
		:mListBox(listBox), mIsDestroyed(false)
	{

	}

	void ScriptGUIListBox::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUIListBox", &ScriptGUIListBox::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIListBox::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIListBox::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIListBox::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_SetElements", &ScriptGUIListBox::internal_setElements);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIListBox::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUIListBox::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUIListBox::internal_setParent);

		onSelectionChangedThunk = (OnSelectionChangedThunkDef)metaData.scriptClass->getMethod("DoOnSelectionChanged", 1).getThunk();
	}

	void ScriptGUIListBox::destroy()
	{
		if(!mIsDestroyed)
		{
			GUIElement::destroy(mListBox);
			mListBox = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUIListBox::internal_createInstance(MonoObject* instance, MonoArray* elements, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 optionsArrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < optionsArrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

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

		GUIListBox* guiListBox = GUIListBox::create(nativeElements, options, toString(MonoUtil::monoToWString(style)));
		guiListBox->onSelectionChanged.connect(std::bind(&ScriptGUIListBox::onSelectionChanged, instance, std::placeholders::_1));

		ScriptGUIListBox* nativeInstance = new (cm_alloc<ScriptGUIListBox>()) ScriptGUIListBox(guiListBox);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
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
		nativeInstance->destroy();
	}

	void ScriptGUIListBox::internal_destroyInstance(ScriptGUIListBox* nativeInstance)
	{
		nativeInstance->destroy();
		cm_delete(nativeInstance);
	}

	void ScriptGUIListBox::internal_setVisible(ScriptGUIListBox* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->getInternalValue()->enableRecursively();
		else
			nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUIListBox::internal_setParent(ScriptGUIListBox* nativeInstance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(nativeInstance->getInternalValue());
	}

	void ScriptGUIListBox::onSelectionChanged(MonoObject* instance, CM::UINT32 index)
	{
		MonoException* exception = nullptr;
		onSelectionChangedThunk(instance, index, &exception);

		MonoUtil::throwIfException(exception);
	}
}