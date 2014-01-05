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

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIToggle::OnClickThunkDef ScriptGUIToggle::onClickThunk;
	ScriptGUIToggle::OnHoverThunkDef ScriptGUIToggle::onHoverThunk;
	ScriptGUIToggle::OnOutThunkDef ScriptGUIToggle::onOutThunk;
	ScriptGUIToggle::OnToggledThunkDef ScriptGUIToggle::onToggledThunk;

	ScriptGUIToggle::ScriptGUIToggle(GUIToggle* toggle)
		:mToggle(toggle)
	{

	}

	void ScriptGUIToggle::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIToggle", &ScriptGUIToggle::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIToggle::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIToggle::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIToggle::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUIToggle::internal_setContent);
		metaData.scriptClass->addInternalCall("Internal_ToggleOn", &ScriptGUIToggle::internal_toggleOn);
		metaData.scriptClass->addInternalCall("Internal_ToggleOff", &ScriptGUIToggle::internal_toggleOff);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIToggle::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_Enable", &ScriptGUIToggle::internal_enable);
		metaData.scriptClass->addInternalCall("Internal_Disable", &ScriptGUIToggle::internal_disable);

		onClickThunk = (OnClickThunkDef)metaData.scriptClass->getMethod("DoOnClick").getThunk();
		onHoverThunk = (OnHoverThunkDef)metaData.scriptClass->getMethod("DoOnHover").getThunk();
		onOutThunk = (OnOutThunkDef)metaData.scriptClass->getMethod("DoOnOut").getThunk();
		onToggledThunk = (OnToggledThunkDef)metaData.scriptClass->getMethod("DoOnToggled", 1).getThunk();
	}

	void ScriptGUIToggle::internal_createInstance(MonoObject* instance, MonoObject* parentLayout, MonoObject* content, 
		MonoObject* toggleGroup, MonoObject* style, MonoArray* guiOptions)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIElementStyle* elemStyle = nullptr;
		if(style != nullptr)
			elemStyle = ScriptGUIElementStyle::toNative(style)->getInternalValue();

		ScriptGUIToggleGroup* scriptToggleGroup = nullptr;
		if(scriptToggleGroup != nullptr)
			scriptToggleGroup = ScriptGUIToggleGroup::toNative(toggleGroup);

		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		GUIToggle* guiToggle = GUIToggle::create(scriptLayout->getParentWidget(), nativeContent, scriptToggleGroup->getInternalValue(), options, elemStyle);

		guiToggle->onClick.connect(std::bind(&ScriptGUIToggle::onClick, instance));
		guiToggle->onHover.connect(std::bind(&ScriptGUIToggle::onHover, instance));
		guiToggle->onOut.connect(std::bind(&ScriptGUIToggle::onOut, instance));
		guiToggle->onToggled.connect(std::bind(&ScriptGUIToggle::onToggled, instance, std::placeholders::_1));

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(guiToggle);

		ScriptGUIToggle* nativeInstance = new (cm_alloc<ScriptGUIToggle>()) ScriptGUIToggle(guiToggle);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIToggle::internal_destroyInstance(ScriptGUIToggle* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	void ScriptGUIToggle::internal_setContent(ScriptGUIToggle* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		nativeInstance->getInternalValue()->setContent(nativeContent);
	}

	void ScriptGUIToggle::internal_toggleOn(ScriptGUIToggle* nativeInstance)
	{
		nativeInstance->getInternalValue()->toggleOn();
	}

	void ScriptGUIToggle::internal_toggleOff(ScriptGUIToggle* nativeInstance)
	{
		nativeInstance->getInternalValue()->toggleOff();
	}

	void ScriptGUIToggle::internal_destroy(ScriptGUIToggle* nativeInstance)
	{
		GUIElement::destroy(nativeInstance->getInternalValue());
	}

	void ScriptGUIToggle::internal_disable(ScriptGUIToggle* nativeInstance)
	{
		nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUIToggle::internal_enable(ScriptGUIToggle* nativeInstance)
	{
		nativeInstance->getInternalValue()->enableRecursively();
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