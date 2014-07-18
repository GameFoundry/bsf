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
		:TScriptGUIElement(instance, foldout)
	{

	}

	void ScriptGUIFoldout::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIFoldout::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUIFoldout::internal_setContent);
		metaData.scriptClass->addInternalCall("Internal_SetExpanded", &ScriptGUIFoldout::internal_setExpanded);
		metaData.scriptClass->addInternalCall("Internal_IsExpanded", &ScriptGUIFoldout::internal_getIsExpanded);

		onToggledThunk = (OnToggledThunkDef)metaData.scriptClass->getMethod("DoOnToggled", 1).getThunk();
	}

	void ScriptGUIFoldout::internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		HString label = ScriptGUIContent::getText(content);
		GUIFoldout* guiFoldout = GUIFoldout::create(label);

		guiFoldout->onStateChanged.connect(std::bind(&ScriptGUIFoldout::onToggled, instance, _1));

		ScriptGUIFoldout* nativeInstance = new (bs_alloc<ScriptGUIFoldout>()) ScriptGUIFoldout(instance, guiFoldout);
	}

	void ScriptGUIFoldout::internal_setContent(ScriptGUIFoldout* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		
		// TODO - Update GUIFoldout once it has a label
	}

	void ScriptGUIFoldout::internal_setExpanded(ScriptGUIFoldout* nativeInstance, bool expanded)
	{
		GUIFoldout* foldout = static_cast<GUIFoldout*>(nativeInstance->getGUIElement());

		foldout->setExpanded(expanded);
	}

	void ScriptGUIFoldout::internal_getIsExpanded(ScriptGUIFoldout* nativeInstance, bool* isExpanded)
	{
		GUIFoldout* foldout = static_cast<GUIFoldout*>(nativeInstance->getGUIElement());

		*isExpanded = foldout->isExpanded();
	}

	void ScriptGUIFoldout::onToggled(MonoObject* instance, bool expanded)
	{
		MonoException* exception = nullptr;
		onToggledThunk(instance, expanded, &exception);

		MonoUtil::throwIfException(exception);
	}
}