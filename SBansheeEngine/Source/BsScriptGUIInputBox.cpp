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

namespace BansheeEngine
{
	ScriptGUIInputBox::ScriptGUIInputBox(MonoObject* instance, GUIInputBox* inputBox)
		:ScriptObject(instance), mInputBox(inputBox), mIsDestroyed(false)
	{

	}

	void ScriptGUIInputBox::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIInputBox::internal_createInstance);
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

	void ScriptGUIInputBox::_onManagedInstanceDeleted()
	{
		destroy();

		ScriptObject::_onManagedInstanceDeleted();
	}

	void ScriptGUIInputBox::internal_createInstance(MonoObject* instance, bool multiline, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for(UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		GUIInputBox* guiInputBox = GUIInputBox::create(multiline, options, toString(MonoUtil::monoToWString(style)));

		ScriptGUIInputBox* nativeInstance = new (cm_alloc<ScriptGUIInputBox>()) ScriptGUIInputBox(instance, guiInputBox);
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