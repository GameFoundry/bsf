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
		metaData.scriptClass->addInternalCall("Internal_GetBounds", &ScriptGUIElement::internal_getBounds);
		metaData.scriptClass->addInternalCall("Internal_SetBounds", &ScriptGUIElement::internal_setBounds);
		metaData.scriptClass->addInternalCall("Internal_GetVisibleBounds", &ScriptGUIElement::internal_getVisibleBounds);
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

	Rect2I ScriptGUIElement::internal_getBounds(ScriptGUIElementBaseTBase* nativeInstance)
	{
		return nativeInstance->getGUIElement()->getBounds();
	}

	void ScriptGUIElement::internal_setBounds(ScriptGUIElementBaseTBase* nativeInstance, Rect2I bounds)
	{
		nativeInstance->getGUIElement()->setOffset(Vector2I(bounds.x, bounds.y));
		nativeInstance->getGUIElement()->setWidth(bounds.width);
		nativeInstance->getGUIElement()->setHeight(bounds.height);
	}

	Rect2I ScriptGUIElement::internal_getVisibleBounds(ScriptGUIElementBaseTBase* nativeInstance)
	{
		return nativeInstance->getGUIElement()->getVisibleBounds();
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