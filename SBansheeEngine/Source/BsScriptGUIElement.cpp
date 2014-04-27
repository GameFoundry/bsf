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
	ScriptGUIElementBase::ScriptGUIElementBase(GUIElement* element)
		:mIsDestroyed(false), mElement(element)
	{ }

	ScriptGUIElement::ScriptGUIElement(MonoObject* instance)
		:ScriptObject(instance)
	{

	}

	void ScriptGUIElementBase::destroy()
	{
		if(!mIsDestroyed)
		{
			GUIElement::destroy(mElement);
			mElement = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUIElement::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIElement::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUIElement::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUIElement::internal_setParent);
	}

	void ScriptGUIElement::internal_destroy(ScriptGUIElementBase* nativeInstance)
	{
		nativeInstance->destroy();
	}

	void ScriptGUIElement::internal_setVisible(ScriptGUIElementBase* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->getGUIElement()->enableRecursively();
		else
			nativeInstance->getGUIElement()->disableRecursively();
	}

	void ScriptGUIElement::internal_setParent(ScriptGUIElementBase* nativeInstance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(nativeInstance->getGUIElement());
	}
}