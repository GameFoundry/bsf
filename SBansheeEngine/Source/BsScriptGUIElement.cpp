#include "BsScriptGUIElement.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsGUIElement.h"
#include "BsScriptGUILayout.h"
#include "BsGUILayout.h"

namespace BansheeEngine
{
	ScriptGUIElementBaseTBase::ScriptGUIElementBaseTBase(MonoObject* instance)
		:ScriptObjectBase(instance), mIsDestroyed(false), mElement(nullptr)
	{ }



	void ScriptGUIElementBaseTBase::initialize(GUIElementBase* element)
	{
		mElement = element;
	}

	ScriptGUIElementTBase::ScriptGUIElementTBase(MonoObject* instance)
		:ScriptGUIElementBaseTBase(instance)
	{

	}

	void ScriptGUIElementTBase::destroy()
	{
		if(!mIsDestroyed)
		{
			if (mElement->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement::destroy((GUIElement*)mElement);
				mElement = nullptr;

				mIsDestroyed = true;
			}
		}
	}

	void ScriptGUIElementTBase::setParent(GUILayout* layout)
	{
		layout->addElement((GUIElement*)mElement);
	}

	void ScriptGUIElementTBase::setLayoutOptions(GUIOptions options)
	{
		GUIElement* element = static_cast<GUIElement*>(mElement);
		element->setLayoutOptions(options);
	}

	ScriptGUIElement::ScriptGUIElement(MonoObject* instance)
		:ScriptObject(instance)
	{

	}

	void ScriptGUIElement::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIElement::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUIElement::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUIElement::internal_setParent);
		metaData.scriptClass->addInternalCall("Internal_SetLayoutOptions", &ScriptGUIElement::internal_setLayoutOptions);
	}

	void ScriptGUIElement::internal_destroy(ScriptGUIElementBaseTBase* nativeInstance)
	{
		nativeInstance->destroy();
	}

	void ScriptGUIElement::internal_setVisible(ScriptGUIElementBaseTBase* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->getGUIElement()->enableRecursively();
		else
			nativeInstance->getGUIElement()->disableRecursively();
	}

	void ScriptGUIElement::internal_setParent(ScriptGUIElementBaseTBase* nativeInstance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();

		nativeInstance->setParent(nativeLayout);
	}

	void ScriptGUIElement::internal_setLayoutOptions(ScriptGUIElementBaseTBase* nativeInstance, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		nativeInstance->setLayoutOptions(options);
	}
}