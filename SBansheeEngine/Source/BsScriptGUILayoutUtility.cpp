#include "BsScriptGUILayoutUtility.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptGUIElement.h"
#include "BsGUILayoutUtility.h"

namespace BansheeEngine
{
	ScriptGUILayoutUtility::ScriptGUILayoutUtility()
		:ScriptObject(nullptr)
	{ }

	void ScriptGUILayoutUtility::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CalculateOptimalSize", &ScriptGUILayoutUtility::internal_CalculateOptimalSize);
		metaData.scriptClass->addInternalCall("Internal_CalculateBounds", &ScriptGUILayoutUtility::internal_CalculateBounds);
	}

	void ScriptGUILayoutUtility::internal_CalculateOptimalSize(ScriptGUIElementBaseTBase* guiElement, Vector2I* output)
	{
		*output = GUILayoutUtility::calcOptimalSize(guiElement->getGUIElement());;
	}

	void ScriptGUILayoutUtility::internal_CalculateBounds(ScriptGUIElementBaseTBase* guiElement, RectI* output)
	{
		*output = GUILayoutUtility::calcBounds(guiElement->getGUIElement());
	}
}