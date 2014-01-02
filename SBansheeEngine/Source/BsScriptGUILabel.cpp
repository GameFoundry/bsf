#include "BsScriptGUILabel.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIOptions.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUILabel::ScriptGUILabel(GUILabel* label)
		:mLabel(label)
	{

	}

	void ScriptGUILabel::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUILabel", &ScriptGUILabel::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}
	void ScriptGUILabel::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUILabel::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUILabel::internal_destroyInstance);
	}

	void ScriptGUILabel::internal_createInstance(MonoObject* instance, MonoObject* parentLayout, MonoObject* content, MonoObject* style, MonoArray* guiOptions)
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
		GUILabel* guiLabel = GUILabel::create(scriptLayout->getParentArea()->getParentWidget(), nativeContent, options, elemStyle); // TODO - Use proper HString
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(guiLabel);

		ScriptGUILabel* nativeInstance = new (cm_alloc<ScriptGUILabel>()) ScriptGUILabel(guiLabel);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUILabel::internal_destroyInstance(ScriptGUILabel* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}