#include "BsScriptGUITexture.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIOptions.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUITexture::ScriptGUITexture(GUITexture* texture)
		:mTexture(texture)
	{

	}

	void ScriptGUITexture::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUITexture", &ScriptGUITexture::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUITexture::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUITexture::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUITexture::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_SetTexture", &ScriptGUITexture::internal_setTexture);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUITexture::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_Enable", &ScriptGUITexture::internal_enable);
		metaData.scriptClass->addInternalCall("Internal_Disable", &ScriptGUITexture::internal_disable);
	}

	void ScriptGUITexture::internal_createInstance(MonoObject* instance, MonoObject* parentLayout, MonoObject* texture, 
		GUIImageScaleMode scale, MonoObject* style, MonoArray* guiOptions)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIElementStyle* elemStyle = nullptr;

		if(style != nullptr)
			elemStyle = ScriptGUIElementStyle::toNative(style)->getInternalValue();

		HSpriteTexture nativeTexture;
		if(texture != nullptr)
			nativeTexture = ScriptSpriteTexture::toNative(texture)->getInternalValue();

		GUITexture* guiTexture = GUITexture::create(scriptLayout->getParentWidget(), nativeTexture, scale, options, elemStyle);
		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(guiTexture);

		ScriptGUITexture* nativeInstance = new (cm_alloc<ScriptGUITexture>()) ScriptGUITexture(guiTexture);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUITexture::internal_destroyInstance(ScriptGUITexture* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	void ScriptGUITexture::internal_setTexture(ScriptGUITexture* nativeInstance, MonoObject* texture)
	{
		HSpriteTexture nativeTexture;
		if(texture != nullptr)
			nativeTexture = ScriptSpriteTexture::toNative(texture)->getInternalValue();

		nativeInstance->getInternalValue()->setTexture(nativeTexture);
	}

	void ScriptGUITexture::internal_destroy(ScriptGUITexture* nativeInstance)
	{
		GUIElement::destroy(nativeInstance->getInternalValue());
	}

	void ScriptGUITexture::internal_disable(ScriptGUITexture* nativeInstance)
	{
		nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUITexture::internal_enable(ScriptGUITexture* nativeInstance)
	{
		nativeInstance->getInternalValue()->enableRecursively();
	}
}