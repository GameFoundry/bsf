#include "BsScriptGUIInputBox.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIInputBox.h"
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
	ScriptGUIInputBox::ScriptGUIInputBox(GUIInputBox* inputBox)
		:mInputBox(inputBox), mIsDestroyed(false)
	{

	}

	void ScriptGUIInputBox::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUITextBox", &ScriptGUIInputBox::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIInputBox::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIInputBox::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIInputBox::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_GetText", &ScriptGUIInputBox::internal_getText);
		metaData.scriptClass->addInternalCall("Internal_SetText", &ScriptGUIInputBox::internal_setText);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIInputBox::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUIInputBox::internal_setVisible);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptGUIInputBox::internal_setParent);
	}

	void ScriptGUIInputBox::destroy()
	{
		if(!mIsDestroyed)
		{
			GUIElement::destroy(mInputBox);
			mInputBox = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUIInputBox::internal_createInstance(MonoObject* instance, bool multiline, MonoObject* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIElementStyle* elemStyle = nullptr;

		if(style != nullptr)
			elemStyle = ScriptGUIElementStyle::toNative(style)->getInternalValue();

		GUIInputBox* guiInputBox = GUIInputBox::create(multiline, options, elemStyle);

		ScriptGUIInputBox* nativeInstance = new (cm_alloc<ScriptGUIInputBox>()) ScriptGUIInputBox(guiInputBox);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
	}

	void ScriptGUIInputBox::internal_getText(ScriptGUIInputBox* nativeInstance, MonoString** text)
	{
		*text = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), nativeInstance->getInternalValue()->getText());
	}

	void ScriptGUIInputBox::internal_setText(ScriptGUIInputBox* nativeInstance, MonoString* text)
	{
		nativeInstance->getInternalValue()->setText(MonoUtil::monoToWString(text));
	}

	void ScriptGUIInputBox::internal_destroy(ScriptGUIInputBox* nativeInstance)
	{
		nativeInstance->destroy();
	}

	void ScriptGUIInputBox::internal_destroyInstance(ScriptGUIInputBox* nativeInstance)
	{
		nativeInstance->destroy();
		cm_delete(nativeInstance);
	}

	void ScriptGUIInputBox::internal_setVisible(ScriptGUIInputBox* nativeInstance, bool visible)
	{
		if(visible)
			nativeInstance->getInternalValue()->enableRecursively();
		else
			nativeInstance->getInternalValue()->disableRecursively();
	}

	void ScriptGUIInputBox::internal_setParent(ScriptGUIInputBox* nativeInstance, MonoObject* parentLayout)
	{
		ScriptGUILayout* scriptLayout = ScriptGUILayout::toNative(parentLayout);

		GUILayout* nativeLayout = scriptLayout->getInternalValue();
		nativeLayout->addElement(nativeInstance->getInternalValue());
	}
}