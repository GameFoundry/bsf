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
		:mLabel(label), mIsDestroyed(false)
	{

	}

	void ScriptGUILabel::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUILabel", &ScriptGUILabel::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUILabel::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUILabel::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUILabel::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_SetContent", &ScriptGUILabel::internal_setContent);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUILabel::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUILabel::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUILabel::internal_setParent);
	}

	void ScriptGUILabel::destroy()
	{
		if(!mIsDestroyed)
		{
			GUIElement::destroy(mLabel);
			mLabel = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUILabel::internal_createInstance(MonoObject* instance, MonoObject* content, MonoObject* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIElementStyle* elemStyle = nullptr;
		
		if(style != nullptr)
			elemStyle = ScriptGUIElementStyle::toNative(style)->getInternalValue();

		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		GUILabel* guiLabel = GUILabel::create(nativeContent, options, elemStyle);

		ScriptGUILabel* nativeInstance = new (cm_alloc<ScriptGUILabel>()) ScriptGUILabel(guiLabel);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
	}

	void ScriptGUILabel::internal_setContent(ScriptGUILabel* nativeInstance, MonoObject* content)
	{
		GUIContent nativeContent(ScriptGUIContent::getText(content), ScriptGUIContent::getImage(content), ScriptGUIContent::getTooltip(content));
		nativeInstance->getInternalValue()->setContent(nativeContent);
	}

	void ScriptGUILabel::internal_destroy(ScriptGUILabel* nativeInstance)
	{
		nativeInstance->destroy();
	}

	void ScriptGUILabel::internal_destroyInstance(ScriptGUILabel* nativeInstance)
	{
		nativeInstance->destroy();
		cm_delete(nativeInstance);
	}

	void ScriptGUILabel::internal_setVisible(ScriptGUILabel* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->getInternalValue()->enableRecursively();
		else
			nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUILabel::internal_setParent(ScriptGUILabel* nativeInstance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(nativeInstance->getInternalValue());
	}
}