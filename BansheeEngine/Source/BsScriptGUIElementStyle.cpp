#include "BsScriptGUIElementStyle.h"
#include "BsScriptMeta.h"
#include "BsScriptField.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
#include "BsScriptFont.h"
#include "CmException.h"
#include "BsGUIElementStyle.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIElementStyle::ScriptGUIElementStyle(const String& name)
		:mName(name), mElementStyle(cm_new<GUIElementStyle>()), mFont(nullptr), mOwnsStyle(true)
	{

	}

	ScriptGUIElementStyle::ScriptGUIElementStyle(const String& name, GUIElementStyle* externalStyle)
		:mName(name), mElementStyle(externalStyle), mFont(nullptr), mOwnsStyle(false)
	{

	}

	ScriptGUIElementStyle::~ScriptGUIElementStyle()
	{
		if(mOwnsStyle)
			cm_delete(mElementStyle);
	}

	void ScriptGUIElementStyle::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIElementStyle", &ScriptGUIElementStyle::initRuntimeData);

		ScriptManager::registerScriptType(&metaData);
	}

	void ScriptGUIElementStyle::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIElementStyle::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIElementStyle::internal_destroyInstance);
	}

	void ScriptGUIElementStyle::internal_createInstance(MonoObject* instance, MonoString* name)
	{
		char* nativeName = mono_string_to_utf8(name);
		String styleName(nativeName);
		free(nativeName);

		ScriptGUIElementStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStyle>()) ScriptGUIElementStyle(styleName);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIElementStyle::internal_createInstanceExternal(MonoObject* instance, MonoString* name, GUIElementStyle* externalStyle)
	{
		char* nativeName = mono_string_to_utf8(name);
		String styleName(nativeName);
		free(nativeName);

		ScriptGUIElementStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStyle>()) ScriptGUIElementStyle(styleName, externalStyle);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIElementStyle::internal_destroyInstance(ScriptGUIElementStyle* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	/*MonoObject* ScriptGUIElementStyle::internal_GetFont(ScriptGUIElementStyle* nativeInstance)
	{
		throwIfInstancesDontMatch(nativeInstance->mFont, nativeInstance->mElementStyle->font.get());

		if(nativeInstance->mFont != nullptr)
			return nativeInstance->mFont->getManagedInstance();

		return nullptr;
	}

	void ScriptGUIElementStyle::internal_SetFont(ScriptGUIElementStyle* nativeInstance, ScriptFont* value)
	{
		nativeInstance->mElementStyle->font = value->getInternalFont();
	}


	int ScriptGUIElementStyle::internal_GetFontSize(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetFontSize(ScriptGUIElementStyle* nativeInstance, int value)
	{

	}


	int ScriptGUIElementStyle::internal_GetTextHorzAlign(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetTextHorzAlign(ScriptGUIElementStyle* nativeInstance, int value)
	{

	}


	int ScriptGUIElementStyle::internal_GetTextVertAlign(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetTextVertAlign(ScriptGUIElementStyle* nativeInstance, int value)
	{

	}


	int ScriptGUIElementStyle::internal_GetImagePosition(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetImagePosition(ScriptGUIElementStyle* nativeInstance, int value)
	{

	}


	bool ScriptGUIElementStyle::internal_GetWordWrap(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetWordWrap(ScriptGUIElementStyle* nativeInstance, bool value)
	{

	}


	MonoObject* ScriptGUIElementStyle::internal_GetNormal(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetNormal(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{

	}


	MonoObject* ScriptGUIElementStyle::internal_GetHover(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetHover(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{

	}


	MonoObject* ScriptGUIElementStyle::internal_GetActive(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetActive(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{

	}


	MonoObject* ScriptGUIElementStyle::internal_GetFocused(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetFocused(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{

	}


	MonoObject* ScriptGUIElementStyle::internal_GetNormalOn(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetNormalOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{

	}


	MonoObject* ScriptGUIElementStyle::internal_GetHoverOn(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetHoverOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{

	}


	MonoObject* ScriptGUIElementStyle::internal_GetActiveOn(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetActiveOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{

	}


	MonoObject* ScriptGUIElementStyle::internal_GetFocusedOn(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetFocusedOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{

	}


	RectOffset ScriptGUIElementStyle::internal_GetBorder(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetBorder(ScriptGUIElementStyle* nativeInstance, RectOffset value)
	{

	}


	RectOffset ScriptGUIElementStyle::internal_GetMargins(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetMargins(ScriptGUIElementStyle* nativeInstance, RectOffset value)
	{

	}


	RectOffset ScriptGUIElementStyle::internal_GetContentOffset(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetContentOffset(ScriptGUIElementStyle* nativeInstance, RectOffset value)
	{

	}


	INT32 ScriptGUIElementStyle::internal_GetWidth(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetWidth(ScriptGUIElementStyle* nativeInstance, INT32 value)
	{

	}


	INT32 ScriptGUIElementStyle::internal_GetHeight(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetHeight(ScriptGUIElementStyle* nativeInstance, INT32 value)
	{

	}


	INT32 ScriptGUIElementStyle::internal_GetMinWidth(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetMinWidth(ScriptGUIElementStyle* nativeInstance, INT32 value)
	{

	}


	INT32 ScriptGUIElementStyle::internal_GetMaxWidth(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetMaxWidth(ScriptGUIElementStyle* nativeInstance, INT32 value)
	{

	}


	INT32 ScriptGUIElementStyle::internal_GetMinHeight(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetMinHeight(ScriptGUIElementStyle* nativeInstance, INT32 value)
	{

	}


	INT32 ScriptGUIElementStyle::internal_GetMaxHeight(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetMaxHeight(ScriptGUIElementStyle* nativeInstance, INT32 value)
	{

	}


	bool ScriptGUIElementStyle::internal_GetFixedWidth(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetFixedWidth(ScriptGUIElementStyle* nativeInstance, bool value)
	{

	}


	bool ScriptGUIElementStyle::internal_GetFixedHeight(ScriptGUIElementStyle* nativeInstance)
	{

	}

	void ScriptGUIElementStyle::internal_SetFixedHeight(ScriptGUIElementStyle* nativeInstance, bool value)
	{

	}*/
}