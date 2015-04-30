#include "BsScriptGUIComponentFoldout.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIComponentFoldout.h"
#include "BsGUIOptions.h"
#include "BsGUIContent.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIComponentFoldout::OnToggledThunkDef ScriptGUIComponentFoldout::onToggledThunk;

	ScriptGUIComponentFoldout::ScriptGUIComponentFoldout(MonoObject* instance, GUIComponentFoldout* foldout)
		:TScriptGUIElement(instance, foldout)
	{

	}

	void ScriptGUIComponentFoldout::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIComponentFoldout::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUIComponentFoldout::internal_setContent);
		metaData.scriptClass->addInternalCall("Internal_SetExpanded", &ScriptGUIComponentFoldout::internal_setExpanded);
		metaData.scriptClass->addInternalCall("Internal_IsExpanded", &ScriptGUIComponentFoldout::internal_getIsExpanded);
		metaData.scriptClass->addInternalCall("Internal_SetTint", &ScriptGUIComponentFoldout::internal_setTint);

		onToggledThunk = (OnToggledThunkDef)metaData.scriptClass->getMethod("DoOnToggled", 1)->getThunk();
	}

	void ScriptGUIComponentFoldout::internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		HString label = ScriptGUIContent::getText(content);
		GUIComponentFoldout* guiFoldout = GUIComponentFoldout::create(label);

		guiFoldout->onStateChanged.connect(std::bind(&ScriptGUIComponentFoldout::onToggled, instance, _1));

		ScriptGUIComponentFoldout* nativeInstance = new (bs_alloc<ScriptGUIComponentFoldout>()) ScriptGUIComponentFoldout(instance, guiFoldout);
	}

	void ScriptGUIComponentFoldout::internal_setContent(ScriptGUIComponentFoldout* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		
		GUIComponentFoldout* guiFoldout = static_cast<GUIComponentFoldout*>(nativeInstance->getGUIElement());
		guiFoldout->setContent(nativeContent);
	}

	void ScriptGUIComponentFoldout::internal_setExpanded(ScriptGUIComponentFoldout* nativeInstance, bool expanded)
	{
		GUIComponentFoldout* foldout = static_cast<GUIComponentFoldout*>(nativeInstance->getGUIElement());
		foldout->setExpanded(expanded);
	}

	void ScriptGUIComponentFoldout::internal_getIsExpanded(ScriptGUIComponentFoldout* nativeInstance, bool* isExpanded)
	{
		GUIComponentFoldout* foldout = static_cast<GUIComponentFoldout*>(nativeInstance->getGUIElement());
		*isExpanded = foldout->isExpanded();
	}

	void ScriptGUIComponentFoldout::internal_setTint(ScriptGUIComponentFoldout* nativeInstance, Color color)
	{
		GUIComponentFoldout* foldout = (GUIComponentFoldout*)nativeInstance->getGUIElement();
		foldout->setTint(color);
	}

	void ScriptGUIComponentFoldout::onToggled(MonoObject* instance, bool expanded)
	{
		MonoException* exception = nullptr;
		onToggledThunk(instance, expanded, &exception);

		MonoUtil::throwIfException(exception);
	}
}