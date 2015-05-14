#include "BsScriptGUIElement.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsGUIElement.h"
#include "BsScriptGUILayout.h"
#include "BsScriptContextMenu.h"
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
		metaData.scriptClass->addInternalCall("Internal_SetPosition", &ScriptGUIElement::internal_SetPosition);
		metaData.scriptClass->addInternalCall("Internal_SetWidth", &ScriptGUIElement::internal_SetWidth);
		metaData.scriptClass->addInternalCall("Internal_SetFlexibleWidth", &ScriptGUIElement::internal_SetFlexibleWidth);
		metaData.scriptClass->addInternalCall("Internal_SetHeight", &ScriptGUIElement::internal_SetHeight);
		metaData.scriptClass->addInternalCall("Internal_SetFlexibleHeight", &ScriptGUIElement::internal_SetFlexibleHeight);
		metaData.scriptClass->addInternalCall("Internal_ResetDimensions", &ScriptGUIElement::internal_ResetDimensions);
		metaData.scriptClass->addInternalCall("Internal_SetContextMenu", &ScriptGUIElement::internal_SetContextMenu);
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
		nativeInstance->getGUIElement()->setPosition(bounds.x, bounds.y);
		nativeInstance->getGUIElement()->setWidth(bounds.width);
		nativeInstance->getGUIElement()->setHeight(bounds.height);
	}

	Rect2I ScriptGUIElement::internal_getVisibleBounds(ScriptGUIElementBaseTBase* nativeInstance)
	{
		return nativeInstance->getGUIElement()->getVisibleBounds();
	}

	void ScriptGUIElement::internal_SetPosition(ScriptGUIElementBaseTBase* nativeInstance, INT32 x, INT32 y)
	{
		nativeInstance->getGUIElement()->setPosition(x, y);
	}

	void ScriptGUIElement::internal_SetWidth(ScriptGUIElementBaseTBase* nativeInstance, UINT32 width)
	{
		nativeInstance->getGUIElement()->setWidth(width);
	}

	void ScriptGUIElement::internal_SetFlexibleWidth(ScriptGUIElementBaseTBase* nativeInstance, UINT32 minWidth, UINT32 maxWidth)
	{
		nativeInstance->getGUIElement()->setFlexibleWidth(minWidth, maxWidth);
	}

	void ScriptGUIElement::internal_SetHeight(ScriptGUIElementBaseTBase* nativeInstance, UINT32 height)
	{
		nativeInstance->getGUIElement()->setHeight(height);
	}

	void ScriptGUIElement::internal_SetFlexibleHeight(ScriptGUIElementBaseTBase* nativeInstance, UINT32 minHeight, UINT32 maxHeight)
	{
		nativeInstance->getGUIElement()->setFlexibleHeight(minHeight, maxHeight);
	}

	void ScriptGUIElement::internal_ResetDimensions(ScriptGUIElementBaseTBase* nativeInstance)
	{
		nativeInstance->getGUIElement()->resetDimensions();
	}

	void ScriptGUIElement::internal_SetContextMenu(ScriptGUIElementBaseTBase* nativeInstance, ScriptContextMenu* contextMenu)
	{
		GUIElementBase* guiElemBase = nativeInstance->getGUIElement();
		if (guiElemBase->_getType() == GUIElementBase::Type::Element)
		{
			GUIElement* guiElem = static_cast<GUIElement*>(guiElemBase);

			GUIContextMenuPtr nativeContextMenu;
			if (contextMenu != nullptr)
				nativeContextMenu = contextMenu->getInternal();

			guiElem->setContextMenu(nativeContextMenu);
		}
	}
}