#include "BsScriptGUIProgressBar.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIProgressBar.h"
#include "BsGUIOptions.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIProgressBar::ScriptGUIProgressBar(MonoObject* instance, GUIProgressBar* progressBar)
		:TScriptGUIElement(instance, progressBar)
	{

	}

	void ScriptGUIProgressBar::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIProgressBar::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetPercent", &ScriptGUIProgressBar::internal_setPercent);
		metaData.scriptClass->addInternalCall("Internal_GetPercent", &ScriptGUIProgressBar::internal_getPercent);
	}

	void ScriptGUIProgressBar::internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIProgressBar* progressBar = GUIProgressBar::create(options, toString(MonoUtil::monoToWString(style)));
		ScriptGUIProgressBar* nativeInstance = new (bs_alloc<ScriptGUIProgressBar>()) ScriptGUIProgressBar(instance, progressBar);
	}

	void ScriptGUIProgressBar::internal_setPercent(ScriptGUIProgressBar* nativeInstance, float percent)
	{
		GUIProgressBar* progressBar = (GUIProgressBar*)nativeInstance->getGUIElement();
		progressBar->setPercent(percent);
	}

	float ScriptGUIProgressBar::internal_getPercent(ScriptGUIProgressBar* nativeInstance)
	{
		GUIProgressBar* progressBar = (GUIProgressBar*)nativeInstance->getGUIElement();
		return progressBar->getPercent();
	}
}