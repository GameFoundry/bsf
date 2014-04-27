#include "BsScriptGUIFoldout.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIFoldout.h"
#include "BsGUIOptions.h"
#include "BsGUIContent.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIFoldout::OnToggledThunkDef ScriptGUIFoldout::onToggledThunk;

	ScriptGUIFoldout::ScriptGUIFoldout(MonoObject* instance, GUIFoldout* foldout)
		:ScriptObject(instance), mFoldout(foldout), mIsDestroyed(false)
	{

	}

	void ScriptGUIFoldout::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIFoldout::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUIFoldout::internal_setContent);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIFoldout::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUIFoldout::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUIFoldout::internal_setParent);

		onToggledThunk = (OnToggledThunkDef)metaData.scriptClass->getMethod("DoOnToggled", 1).getThunk();
	}

	void ScriptGUIFoldout::destroy()
	{
		if(!mIsDestroyed)
		{
			GUIElement::destroy(mFoldout);
			mFoldout = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUIFoldout::_onManagedInstanceDeleted()
	{
		destroy();

		ScriptObject::_onManagedInstanceDeleted();
	}

	void ScriptGUIFoldout::internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		GUIFoldout* guiFoldout = GUIFoldout::create(options, toString(MonoUtil::monoToWString(style))); // TODO - Add label

		guiFoldout->onStateChanged.connect(std::bind(&ScriptGUIFoldout::onToggled, instance, _1));

		ScriptGUIFoldout* nativeInstance = new (cm_alloc<ScriptGUIFoldout>()) ScriptGUIFoldout(instance, guiFoldout);
	}

	void ScriptGUIFoldout::internal_setContent(ScriptGUIFoldout* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		
		// TODO - Update GUIFoldout once it has a label
	}

	void ScriptGUIFoldout::internal_destroy(ScriptGUIFoldout* nativeInstance)
	{
		nativeInstance->destroy();
	}

	void ScriptGUIFoldout::internal_setVisible(ScriptGUIFoldout* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->getInternalValue()->enableRecursively();
		else
			nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUIFoldout::internal_setParent(ScriptGUIFoldout* nativeInstance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(nativeInstance->getInternalValue());
	}

	void ScriptGUIFoldout::onToggled(MonoObject* instance, bool expanded)
	{
		MonoException* exception = nullptr;
		onToggledThunk(instance, expanded, &exception);

		MonoUtil::throwIfException(exception);
	}
}