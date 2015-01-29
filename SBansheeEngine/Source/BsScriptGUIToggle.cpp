#include "BsScriptGUIToggle.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIToggle.h"
#include "BsGUIToggleGroup.h"
#include "BsGUIOptions.h"
#include "BsScriptGUIToggleGroup.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIToggle::OnClickThunkDef ScriptGUIToggle::onClickThunk;
	ScriptGUIToggle::OnHoverThunkDef ScriptGUIToggle::onHoverThunk;
	ScriptGUIToggle::OnOutThunkDef ScriptGUIToggle::onOutThunk;
	ScriptGUIToggle::OnToggledThunkDef ScriptGUIToggle::onToggledThunk;

	ScriptGUIToggle::ScriptGUIToggle(MonoObject* instance, GUIToggle* toggle)
		:TScriptGUIElement(instance, toggle)
	{

	}

	void ScriptGUIToggle::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIToggle::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUIToggle::internal_setContent);
		metaData.scriptClass->addInternalCall("Internal_ToggleOn", &ScriptGUIToggle::internal_toggleOn);
		metaData.scriptClass->addInternalCall("Internal_ToggleOff", &ScriptGUIToggle::internal_toggleOff);

		onClickThunk = (OnClickThunkDef)metaData.scriptClass->getMethod("DoOnClick")->getThunk();
		onHoverThunk = (OnHoverThunkDef)metaData.scriptClass->getMethod("DoOnHover")->getThunk();
		onOutThunk = (OnOutThunkDef)metaData.scriptClass->getMethod("DoOnOut")->getThunk();
		onToggledThunk = (OnToggledThunkDef)metaData.scriptClass->getMethod("DoOnToggled", 1)->getThunk();
	}

	void ScriptGUIToggle::internal_createInstance(MonoObject* instance, MonoObject* content, 
		MonoObject* monoToggleGroup, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		ScriptGUIToggleGroup* scriptToggleGroup = nullptr;
		std::shared_ptr<GUIToggleGroup> toggleGroup;
		if (monoToggleGroup != nullptr)
		{
			scriptToggleGroup = ScriptGUIToggleGroup::toNative(monoToggleGroup);
			toggleGroup = scriptToggleGroup->getInternalValue();
		}

		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		GUIToggle* guiToggle = GUIToggle::create(nativeContent, toggleGroup, options, toString(MonoUtil::monoToWString(style)));

		guiToggle->onClick.connect(std::bind(&ScriptGUIToggle::onClick, instance));
		guiToggle->onHover.connect(std::bind(&ScriptGUIToggle::onHover, instance));
		guiToggle->onOut.connect(std::bind(&ScriptGUIToggle::onOut, instance));
		guiToggle->onToggled.connect(std::bind(&ScriptGUIToggle::onToggled, instance, std::placeholders::_1));

		ScriptGUIToggle* nativeInstance = new (bs_alloc<ScriptGUIToggle>()) ScriptGUIToggle(instance, guiToggle);
	}

	void ScriptGUIToggle::internal_setContent(ScriptGUIToggle* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));

		GUIToggle* toggle = (GUIToggle*)nativeInstance->getGUIElement();
		toggle->setContent(nativeContent);
	}

	void ScriptGUIToggle::internal_toggleOn(ScriptGUIToggle* nativeInstance)
	{
		GUIToggle* toggle = (GUIToggle*)nativeInstance->getGUIElement();
		toggle->toggleOn();
	}

	void ScriptGUIToggle::internal_toggleOff(ScriptGUIToggle* nativeInstance)
	{
		GUIToggle* toggle = (GUIToggle*)nativeInstance->getGUIElement();
		toggle->toggleOff();
	}

	void ScriptGUIToggle::onClick(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		onClickThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptGUIToggle::onHover(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		onHoverThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptGUIToggle::onOut(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		onOutThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptGUIToggle::onToggled(MonoObject* instance, bool toggled)
	{
		MonoException* exception = nullptr;
		onToggledThunk(instance, toggled, &exception);

		MonoUtil::throwIfException(exception);
	}
}