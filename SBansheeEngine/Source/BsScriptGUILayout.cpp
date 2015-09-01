#include "BsScriptGUILayout.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptGUIScrollArea.h"
#include "BsGUILayout.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUIPanel.h"
#include "BsGUIScrollArea.h"

namespace BansheeEngine
{
	ScriptGUILayout::ScriptGUILayout(MonoObject* instance, GUILayout* layout, bool ownsNative)
		:TScriptGUIElementBase(instance, layout), mLayout(layout), mIsDestroyed(false), mOwnsNative(ownsNative)
	{

	}

	void ScriptGUILayout::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceX", &ScriptGUILayout::internal_createInstanceX);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceY", &ScriptGUILayout::internal_createInstanceY);
		metaData.scriptClass->addInternalCall("Internal_CreateInstancePanel", &ScriptGUILayout::internal_createInstancePanel);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceYFromScrollArea", &ScriptGUILayout::internal_createInstanceYFromScrollArea);
		metaData.scriptClass->addInternalCall("Internal_AddElement", &ScriptGUILayout::internal_addElement);
		metaData.scriptClass->addInternalCall("Internal_InsertElement", &ScriptGUILayout::internal_insertElement);
		metaData.scriptClass->addInternalCall("Internal_GetChildCount", &ScriptGUILayout::internal_getChildCount);
		metaData.scriptClass->addInternalCall("Internal_GetChild", &ScriptGUILayout::internal_getChild);
	}

	void ScriptGUILayout::destroy()
	{
		if(!mIsDestroyed)
		{
			if (mParent != nullptr)
				mParent->removeChild(this);

			while (mChildren.size() > 0)
			{
				ChildInfo childInfo = mChildren[0];
				childInfo.element->destroy();
			}

			if (mOwnsNative)
				GUILayout::destroy(mLayout);

			mLayout = nullptr;
			mIsDestroyed = true;
		}
	}

	void ScriptGUILayout::addChild(ScriptGUIElementBaseTBase* element)
	{
		ChildInfo childInfo;

		childInfo.element = element;
		childInfo.gcHandle = mono_gchandle_new(element->getManagedInstance(), false);

		mChildren.push_back(childInfo);
	}

	void ScriptGUILayout::insertChild(UINT32 idx, ScriptGUIElementBaseTBase* element)
	{
		ChildInfo childInfo;

		childInfo.element = element;
		childInfo.gcHandle = mono_gchandle_new(element->getManagedInstance(), false);

		mChildren.insert(mChildren.begin() + idx, childInfo);
	}

	void ScriptGUILayout::removeChild(ScriptGUIElementBaseTBase* element)
	{
		auto iterFind = std::find_if(mChildren.begin(), mChildren.end(), 
			[&](const ChildInfo& x)
		{
			return x.element == element;
		});

		if (iterFind != mChildren.end())
		{
			mono_gchandle_free(iterFind->gcHandle);
			mChildren.erase(iterFind);
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

	void ScriptGUILayout::internal_createInstancePanel(MonoObject* instance, INT16 depth, UINT16 depthRangeMin, UINT32 depthRangeMax, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUILayout* layout = GUIPanel::create(depth, depthRangeMin, depthRangeMax, options);

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) ScriptGUILayout(instance, layout);
	}

	void ScriptGUILayout::internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea)
	{
		ScriptGUIScrollArea* scriptScrollArea = ScriptGUIScrollArea::toNative(parentScrollArea);
		GUIScrollArea* scrollArea = (GUIScrollArea*)scriptScrollArea->getGUIElement();

		GUILayout* nativeLayout = &scrollArea->getLayout();

		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>())
			ScriptGUILayout(instance, nativeLayout, false);

		// This method is expected to be called during GUIScrollArea construction, so we finish its initialization
		scriptScrollArea->initialize(nativeInstance);
	}

	void ScriptGUILayout::internal_addElement(ScriptGUILayout* instance, ScriptGUIElementBaseTBase* element)
	{
		if (instance->isDestroyed() || element->isDestroyed())
			return;

		instance->getInternalValue()->addElement(element->getGUIElement());

		if (element->getParent() != nullptr)
			element->getParent()->removeChild(element);

		element->setParent(instance);
		instance->addChild(element);
	}

	void ScriptGUILayout::internal_insertElement(ScriptGUILayout* instance, UINT32 index, ScriptGUIElementBaseTBase* element)
	{
		if (instance->isDestroyed() || element->isDestroyed())
			return;

		instance->getInternalValue()->insertElement(index, element->getGUIElement());

		if (element->getParent() != nullptr)
			element->getParent()->removeChild(element);

		element->setParent(instance);
		instance->insertChild(index, element);
	}

	UINT32 ScriptGUILayout::internal_getChildCount(ScriptGUILayout* instance)
	{
		if (instance->isDestroyed())
			return 0;

		return instance->mLayout->getNumChildren();
	}

	MonoObject* ScriptGUILayout::internal_getChild(ScriptGUILayout* instance, UINT32 index)
	{
		if (instance->isDestroyed() || instance->mChildren.size() >= index)
			return nullptr;

		return instance->mChildren[index].element->getManagedInstance();
	}

	ScriptGUIPanel::ScriptGUIPanel(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptGUIPanel::initRuntimeData()
	{ }

	MonoObject* ScriptGUIPanel::createFromExisting(GUIPanel* panel)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		ScriptGUILayout* nativeInstance = new (bs_alloc<ScriptGUILayout>()) ScriptGUILayout(managedInstance, panel, false);

		return managedInstance;
	}
}