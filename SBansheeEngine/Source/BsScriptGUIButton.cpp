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

namespace BansheeEngine
{
	ScriptGUIButton::OnClickThunkDef ScriptGUIButton::onClickThunk;
	ScriptGUIButton::OnHoverThunkDef ScriptGUIButton::onHoverThunk;
	ScriptGUIButton::OnOutThunkDef ScriptGUIButton::onOutThunk;

	ScriptGUIButton::ScriptGUIButton(MonoObject* instance, GUIButton* button)
		:ScriptObject(instance), mButton(button), mIsDestroyed(false)
	{

	}

	void ScriptGUIButton::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIButton::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUIButton::internal_setContent);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIButton::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUIButton::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUIButton::internal_setParent);

		onClickThunk = (OnClickThunkDef)metaData.scriptClass->getMethod("DoOnClick").getThunk();
		onHoverThunk = (OnHoverThunkDef)metaData.scriptClass->getMethod("DoOnHover").getThunk();
		onOutThunk = (OnOutThunkDef)metaData.scriptClass->getMethod("DoOnOut").getThunk();
	}

	void ScriptGUIButton::destroy()
	{
		if(!mIsDestroyed)
		{
			GUIElement::destroy(mButton);
			mButton = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUIButton::_onManagedInstanceDeleted()
	{
		destroy();

		ScriptObject::_onManagedInstanceDeleted();
	}

	void ScriptGUIButton::internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		GUIButton* guiButton = GUIButton::create(nativeContent, options, toString(MonoUtil::monoToWString(style)));

		guiButton->onClick.connect(std::bind(&ScriptGUIButton::onClick, instance));
		guiButton->onHover.connect(std::bind(&ScriptGUIButton::onHover, instance));
		guiButton->onOut.connect(std::bind(&ScriptGUIButton::onOut, instance));

		ScriptGUIButton* nativeInstance = new (cm_alloc<ScriptGUIButton>()) ScriptGUIButton(instance, guiButton);
	}

	void ScriptGUIButton::internal_setContent(ScriptGUIButton* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		nativeInstance->getInternalValue()->setContent(nativeContent);
	}

	void ScriptGUIButton::internal_destroy(ScriptGUIButton* nativeInstance)
	{
		nativeInstance->destroy();
	}

	void ScriptGUIButton::internal_setVisible(ScriptGUIButton* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->getInternalValue()->enableRecursively();
		else
			nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUIButton::internal_setParent(ScriptGUIButton* nativeInstance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(nativeInstance->getInternalValue());
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