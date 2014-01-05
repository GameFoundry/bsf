#include "BsScriptGUIButton.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIButton.h"
#include "BsGUIOptions.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIButton::OnClickThunkDef ScriptGUIButton::onClickThunk;
	ScriptGUIButton::OnHoverThunkDef ScriptGUIButton::onHoverThunk;
	ScriptGUIButton::OnOutThunkDef ScriptGUIButton::onOutThunk;

	ScriptGUIButton::ScriptGUIButton(GUIButton* button)
		:mButton(button)
	{

	}

	void ScriptGUIButton::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIButton", &ScriptGUIButton::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIButton::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIButton::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIButton::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUIButton::internal_setContent);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIButton::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_Enable", &ScriptGUIButton::internal_enable);
		metaData.scriptClass->addInternalCall("Internal_Disable", &ScriptGUIButton::internal_disable);

		onClickThunk = (OnClickThunkDef)metaData.scriptClass->getMethod("DoOnClick").getThunk();
		onHoverThunk = (OnHoverThunkDef)metaData.scriptClass->getMethod("DoOnHover").getThunk();
		onOutThunk = (OnOutThunkDef)metaData.scriptClass->getMethod("DoOnOut").getThunk();
	}

	void ScriptGUIButton::internal_createInstance(MonoObject* instance, MonoObject* parentLayout, MonoObject* content, MonoObject* style, MonoArray* guiOptions)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIElementStyle* elemStyle = nullptr;

		if(style != nullptr)
			elemStyle = ScriptGUIElementStyle::toNative(style)->getInternalValue();

		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		GUIButton* guiButton = GUIButton::create(scriptLayout->getParentWidget(), nativeContent, options, elemStyle);

		guiButton->onClick.connect(std::bind(&ScriptGUIButton::onClick, instance));
		guiButton->onHover.connect(std::bind(&ScriptGUIButton::onHover, instance));
		guiButton->onOut.connect(std::bind(&ScriptGUIButton::onOut, instance));

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(guiButton);

		ScriptGUIButton* nativeInstance = new (cm_alloc<ScriptGUIButton>()) ScriptGUIButton(guiButton);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIButton::internal_destroyInstance(ScriptGUIButton* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	void ScriptGUIButton::internal_setContent(ScriptGUIButton* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		nativeInstance->getInternalValue()->setContent(nativeContent);
	}

	void ScriptGUIButton::internal_destroy(ScriptGUIButton* nativeInstance)
	{
		GUIElement::destroy(nativeInstance->getInternalValue());
	}

	void ScriptGUIButton::internal_disable(ScriptGUIButton* nativeInstance)
	{
		nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUIButton::internal_enable(ScriptGUIButton* nativeInstance)
	{
		nativeInstance->getInternalValue()->enableRecursively();
	}

	void ScriptGUIButton::onClick(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		onClickThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptGUIButton::onHover(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		onHoverThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptGUIButton::onOut(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		onOutThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
	}
}