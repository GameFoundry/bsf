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
#include "BsGUILayoutExplicit.h"
#include "BsGUIScrollArea.h"

namespace BansheeEngine
{
	ScriptGUILayout::ScriptGUILayout(MonoObject* instance, GUILayout* layout, GUILayout* parentLayout)
		:TScriptGUIElementBase(instance, layout), mLayout(layout), mParentLayout(parentLayout), mIsDestroyed(false)
	{

	}

	void ScriptGUILayout::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromArea", &ScriptGUILayout::internal_createInstanceXFromArea);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromLayoutAdd", &ScriptGUILayout::internal_createInstanceXFromLayoutAdd);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceYFromLayoutAdd", &ScriptGUILayout::internal_createInstanceYFromLayoutAdd);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceXFromLayoutInsert", &ScriptGUILayout::internal_createInstanceXFromLayoutInsert);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceYFromLayoutInsert", &ScriptGUILayout::internal_createInstanceYFromLayoutInsert);
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
			mParentLayout = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUILayout::internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea)
	{
		ScriptGUIArea* scriptArea = ScriptGUIArea::toNative(parentArea);
		GUIArea* nativeArea = scriptArea->getInternalValue();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(instance, &nativeArea->getLayout(), nullptr);
	}

	void ScriptGUILayout::internal_createInstanceXFromLayoutAdd(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout* layout = nativeLayout->addNewElement<GUILayoutX>();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(instance, layout, nativeLayout);
	}

	void ScriptGUILayout::internal_createInstanceYFromLayoutAdd(MonoObject* instance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout* layout = nativeLayout->addNewElement<GUILayoutY>();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(instance, layout, nativeLayout);
	}

	void ScriptGUILayout::internal_createInstanceXFromLayoutInsert(MonoObject* instance, MonoObject* parentLayout, UINT32 index)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout* layout = nativeLayout->insertNewElement<GUILayoutX>(index);

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>())
			ScriptGUILayout(instance, layout, nativeLayout);
	}

	void ScriptGUILayout::internal_createInstanceYFromLayoutInsert(MonoObject* instance, MonoObject* parentLayout, UINT32 index)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		GUILayout* layout = nativeLayout->insertNewElement<GUILayoutY>(index);

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>())
			ScriptGUILayout(instance, layout, nativeLayout);
	}

	void ScriptGUILayout::internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea)
	{
		ScriptGUIScrollArea* scriptScrollArea = ScriptGUIScrollArea::toNative(parentScrollArea);
		GUIScrollArea* scrollArea = (GUIScrollArea*)scriptScrollArea->getGUIElement();

		GUILayout* nativeLayout = &scrollArea->getLayout();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) 
			ScriptGUILayout(instance, nativeLayout, nullptr);
	}

	void ScriptGUILayout::internal_addElement(ScriptGUILayout* instance, ScriptGUIElementTBase* element)
	{
		instance->getInternalValue()->addElement(static_cast<GUIElement*>(element->getGUIElement()));
	}

	void ScriptGUILayout::internal_insertElement(ScriptGUILayout* instance, UINT32 index, ScriptGUIElementTBase* element)
	{
		instance->getInternalValue()->insertElement(index, static_cast<GUIElement*>(element->getGUIElement()));
	}
}