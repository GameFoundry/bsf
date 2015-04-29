#include "BsScriptGUILayout.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptGUIArea.h"
#include "BsScriptGUIScrollArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUIPanel.h"
#include "BsGUIScrollArea.h"

namespace BansheeEngine
{
	ScriptGUILayout::ScriptGUILayout(MonoObject* instance, GUILayout* layout)
		:TScriptGUIElementBase(instance, layout), mLayout(layout), mIsDestroyed(false)
	{

	}

	void ScriptGUILayout::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceX", &ScriptGUILayout::internal_createInstanceX);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceY", &ScriptGUILayout::internal_createInstanceY);
		metaData.scriptClass->addInternalCall("Internal_CreateInstancePanel", &ScriptGUILayout::internal_createInstancePanel);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromArea", &ScriptGUILayout::internal_createInstanceXFromArea);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceYFromScrollArea", &ScriptGUILayout::internal_createInstanceYFromScrollArea);
		metaData.scriptClass->addInternalCall("Internal_AddElement", &ScriptGUILayout::internal_addElement);
		metaData.scriptClass->addInternalCall("Internal_InsertElement", &ScriptGUILayout::internal_insertElement);
	}

	void ScriptGUILayout::destroy()
	{
		if(!mIsDestroyed)
		{
			GUILayout::destroy(mLayout);

			mLayout = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUILayout::internal_createInstanceX(MonoObject* instance, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUILayout* layout = GUILayoutX::create(options);

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) ScriptGUILayout(instance, layout);
	}

	void ScriptGUILayout::internal_createInstanceY(MonoObject* instance, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUILayout* layout = GUILayoutY::create(options);

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) ScriptGUILayout(instance, layout);
	}

	void ScriptGUILayout::internal_createInstancePanel(MonoObject* instance, UINT16 depth, UINT16 depthRange, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUILayout* layout = GUIPanel::create(depth, depthRange, options);

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) ScriptGUILayout(instance, layout);
	}

	void ScriptGUILayout::internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea)
	{
		ScriptGUIArea* scriptArea = ScriptGUIArea::toNative(parentArea);
		GUIArea* nativeArea = scriptArea->getInternalValue();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(instance, &nativeArea->getLayout());
	}

	void ScriptGUILayout::internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea)
	{
		ScriptGUIScrollArea* scriptScrollArea = ScriptGUIScrollArea::toNative(parentScrollArea);
		GUIScrollArea* scrollArea = (GUIScrollArea*)scriptScrollArea->getGUIElement();

		GUILayout* nativeLayout = &scrollArea->getLayout();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>())
			ScriptGUILayout(instance, nativeLayout);
	}

	void ScriptGUILayout::internal_addElement(ScriptGUILayout* instance, ScriptGUIElementTBase* element)
	{
		instance->getInternalValue()->addElement(element->getGUIElement());
	}

	void ScriptGUILayout::internal_insertElement(ScriptGUILayout* instance, UINT32 index, ScriptGUIElementTBase* element)
	{
		instance->getInternalValue()->insertElement(index, element->getGUIElement());
	}
}