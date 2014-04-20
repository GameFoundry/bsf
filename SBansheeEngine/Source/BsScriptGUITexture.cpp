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

namespace BansheeEngine
{
	ScriptGUITexture::ScriptGUITexture(GUITexture* texture)
		:mTexture(texture), mIsDestroyed(false)
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
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUITexture::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUITexture::internal_setParent);
	}

	void ScriptGUITexture::destroy()
	{
		if(!mIsDestroyed)
		{
			GUIElement::destroy(mTexture);
			mTexture = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUITexture::internal_createInstance(MonoObject* instance, MonoObject* texture, 
		GUIImageScaleMode scale, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		HSpriteTexture nativeTexture;
		if(texture != nullptr)
			nativeTexture = ScriptSpriteTexture::toNative(texture)->getInternalValue();

		GUITexture* guiTexture = GUITexture::create(nativeTexture, scale, options, toString(MonoUtil::monoToWString(style)));

		ScriptGUITexture* nativeInstance = new (cm_alloc<ScriptGUITexture>()) ScriptGUITexture(guiTexture);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
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
		nativeInstance->destroy();
	}

	void ScriptGUITexture::internal_destroyInstance(ScriptGUITexture* nativeInstance)
	{
		nativeInstance->destroy();
		cm_delete(nativeInstance);
	}

	void ScriptGUITexture::internal_setVisible(ScriptGUITexture* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->getInternalValue()->enableRecursively();
		else
			nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUITexture::internal_setParent(ScriptGUITexture* nativeInstance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(nativeInstance->getInternalValue());
	}
}