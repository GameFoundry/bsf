#include "BsScriptGUIElementStyle.h"
#include "BsScriptMeta.h"
#include "BsScriptField.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
#include "BsScriptFont.h"
#include "CmException.h"
#include "BsGUIElementStyle.h"
#include "BsScriptGUIElementStateStyle.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIElementStyle::ScriptGUIElementStyle(const String& name)
		:mName(name), mElementStyle(cm_new<GUIElementStyle>()), mFont(nullptr), mOwnsStyle(true), mNormal(nullptr), mHover(nullptr),
		mActive(nullptr), mFocused(nullptr), mNormalOn(nullptr), mHoverOn(nullptr), mActiveOn(nullptr), mFocusedOn(nullptr)
	{

	}

	ScriptGUIElementStyle::ScriptGUIElementStyle(const String& name, GUIElementStyle* externalStyle)
		:mName(name), mElementStyle(externalStyle), mFont(nullptr), mOwnsStyle(false), mNormal(nullptr), mHover(nullptr),
		mActive(nullptr), mFocused(nullptr), mNormalOn(nullptr), mHoverOn(nullptr), mActiveOn(nullptr), mFocusedOn(nullptr)
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

		metaData.scriptClass->addInternalCall("Internal_GetFont", &ScriptGUIElementStyle::internal_GetFont);
		metaData.scriptClass->addInternalCall("Internal_SetFont", &ScriptGUIElementStyle::internal_SetFont);

		metaData.scriptClass->addInternalCall("Internal_Getfont", &ScriptGUIElementStyle::internal_GetFont);
		metaData.scriptClass->addInternalCall("Internal_Getfont", &ScriptGUIElementStyle::internal_GetFont);
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

	void ScriptGUIElementStyle::internal_GetFont(ScriptGUIElementStyle* nativeInstance, MonoObject** value)
	{
		throwIfInstancesDontMatch(nativeInstance->mFont, nativeInstance->mElementStyle->font.get());

		if(nativeInstance->mFont != nullptr)
		{
			*value = nativeInstance->mFont->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStyle::internal_SetFont(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{
		ScriptFont* nativeValue = ScriptFont::toNative(value);
		nativeInstance->mElementStyle->font = nativeValue->getInternalValue();
	}


	void ScriptGUIElementStyle::internal_GetFontSize(ScriptGUIElementStyle* nativeInstance, CM::UINT32* value)
	{
		*value = nativeInstance->mElementStyle->fontSize;
	}

	void ScriptGUIElementStyle::internal_SetFontSize(ScriptGUIElementStyle* nativeInstance, CM::UINT32 value)
	{
		nativeInstance->mElementStyle->fontSize = value;
	}


	void ScriptGUIElementStyle::internal_GetTextHorzAlign(ScriptGUIElementStyle* nativeInstance, TextHorzAlign* value)
	{
		*value = nativeInstance->mElementStyle->textHorzAlign;
	}

	void ScriptGUIElementStyle::internal_SetTextHorzAlign(ScriptGUIElementStyle* nativeInstance, TextHorzAlign value)
	{
		nativeInstance->mElementStyle->textHorzAlign = value;
	}


	void ScriptGUIElementStyle::internal_GetTextVertAlign(ScriptGUIElementStyle* nativeInstance, TextVertAlign* value)
	{
		*value = nativeInstance->mElementStyle->textVertAlign;
	}

	void ScriptGUIElementStyle::internal_SetTextVertAlign(ScriptGUIElementStyle* nativeInstance, TextVertAlign value)
	{
		nativeInstance->mElementStyle->textVertAlign = value;
	}


	void ScriptGUIElementStyle::internal_GetImagePosition(ScriptGUIElementStyle* nativeInstance, GUIImagePosition* value)
	{
		*value = nativeInstance->mElementStyle->imagePosition;
	}

	void ScriptGUIElementStyle::internal_SetImagePosition(ScriptGUIElementStyle* nativeInstance, GUIImagePosition value)
	{
		nativeInstance->mElementStyle->imagePosition = value;
	}


	void ScriptGUIElementStyle::internal_GetWordWrap(ScriptGUIElementStyle* nativeInstance, bool* value)
	{
		*value = nativeInstance->mElementStyle->wordWrap;
	}

	void ScriptGUIElementStyle::internal_SetWordWrap(ScriptGUIElementStyle* nativeInstance, bool value)
	{
		nativeInstance->mElementStyle->wordWrap = value;
	}

	
	void ScriptGUIElementStyle::internal_GetNormal(ScriptGUIElementStyle* nativeInstance, MonoObject** value)
	{
		throwIfInstancesDontMatch(nativeInstance->mNormal, &nativeInstance->mElementStyle->normal);

		if(nativeInstance->mNormal != nullptr)
		{
			*value = nativeInstance->mNormal->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStyle::internal_SetNormal(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{
		ScriptGUIElementStateStyle* nativeValue = ScriptGUIElementStateStyle::toNative(value);
		nativeInstance->mElementStyle->normal = *nativeValue->getInternalValue();
	}


	void ScriptGUIElementStyle::internal_GetHover(ScriptGUIElementStyle* nativeInstance, MonoObject** value)
	{
		throwIfInstancesDontMatch(nativeInstance->mHover, &nativeInstance->mElementStyle->hover);

		if(nativeInstance->mHover != nullptr)
		{
			*value = nativeInstance->mHover->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStyle::internal_SetHover(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{
		ScriptGUIElementStateStyle* nativeValue = ScriptGUIElementStateStyle::toNative(value);
		nativeInstance->mElementStyle->hover = *nativeValue->getInternalValue();
	}


	void ScriptGUIElementStyle::internal_GetActive(ScriptGUIElementStyle* nativeInstance, MonoObject** value)
	{
		throwIfInstancesDontMatch(nativeInstance->mActive, &nativeInstance->mElementStyle->active);

		if(nativeInstance->mActive != nullptr)
		{
			*value = nativeInstance->mActive->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStyle::internal_SetActive(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{
		ScriptGUIElementStateStyle* nativeValue = ScriptGUIElementStateStyle::toNative(value);
		nativeInstance->mElementStyle->active = *nativeValue->getInternalValue();
	}


	void ScriptGUIElementStyle::internal_GetFocused(ScriptGUIElementStyle* nativeInstance, MonoObject** value)
	{
		throwIfInstancesDontMatch(nativeInstance->mFocused, &nativeInstance->mElementStyle->focused);

		if(nativeInstance->mFocused != nullptr)
		{
			*value = nativeInstance->mFocused->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStyle::internal_SetFocused(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{
		ScriptGUIElementStateStyle* nativeValue = ScriptGUIElementStateStyle::toNative(value);
		nativeInstance->mElementStyle->focused = *nativeValue->getInternalValue();
	}


	void ScriptGUIElementStyle::internal_GetNormalOn(ScriptGUIElementStyle* nativeInstance, MonoObject** value)
	{
		throwIfInstancesDontMatch(nativeInstance->mNormalOn, &nativeInstance->mElementStyle->normalOn);

		if(nativeInstance->mNormalOn != nullptr)
		{
			*value = nativeInstance->mNormalOn->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStyle::internal_SetNormalOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{
		ScriptGUIElementStateStyle* nativeValue = ScriptGUIElementStateStyle::toNative(value);
		nativeInstance->mElementStyle->normalOn = *nativeValue->getInternalValue();
	}


	void ScriptGUIElementStyle::internal_GetHoverOn(ScriptGUIElementStyle* nativeInstance, MonoObject** value)
	{
		throwIfInstancesDontMatch(nativeInstance->mHoverOn, &nativeInstance->mElementStyle->hoverOn);

		if(nativeInstance->mHoverOn != nullptr)
		{
			*value = nativeInstance->mHoverOn->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStyle::internal_SetHoverOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{
		ScriptGUIElementStateStyle* nativeValue = ScriptGUIElementStateStyle::toNative(value);
		nativeInstance->mElementStyle->hoverOn = *nativeValue->getInternalValue();
	}


	void ScriptGUIElementStyle::internal_GetActiveOn(ScriptGUIElementStyle* nativeInstance, MonoObject** value)
	{
		throwIfInstancesDontMatch(nativeInstance->mActiveOn, &nativeInstance->mElementStyle->activeOn);

		if(nativeInstance->mActiveOn != nullptr)
		{
			*value = nativeInstance->mActiveOn->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStyle::internal_SetActiveOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{
		ScriptGUIElementStateStyle* nativeValue = ScriptGUIElementStateStyle::toNative(value);
		nativeInstance->mElementStyle->activeOn = *nativeValue->getInternalValue();
	}


	void ScriptGUIElementStyle::internal_GetFocusedOn(ScriptGUIElementStyle* nativeInstance, MonoObject** value)
	{
		throwIfInstancesDontMatch(nativeInstance->mFocusedOn, &nativeInstance->mElementStyle->focusedOn);

		if(nativeInstance->mFocusedOn != nullptr)
		{
			*value = nativeInstance->mFocusedOn->getManagedInstance();
			return;
		}

		*value = nullptr;
	}

	void ScriptGUIElementStyle::internal_SetFocusedOn(ScriptGUIElementStyle* nativeInstance, MonoObject* value)
	{
		ScriptGUIElementStateStyle* nativeValue = ScriptGUIElementStateStyle::toNative(value);
		nativeInstance->mElementStyle->focusedOn = *nativeValue->getInternalValue();
	}


	void ScriptGUIElementStyle::internal_GetBorder(ScriptGUIElementStyle* nativeInstance, RectOffset* value)
	{
		*value = nativeInstance->mElementStyle->border;
	}

	void ScriptGUIElementStyle::internal_SetBorder(ScriptGUIElementStyle* nativeInstance, RectOffset* value)
	{
		nativeInstance->mElementStyle->border = *value;
	}


	void ScriptGUIElementStyle::internal_GetMargins(ScriptGUIElementStyle* nativeInstance, RectOffset* value)
	{
		*value = nativeInstance->mElementStyle->margins;
	}

	void ScriptGUIElementStyle::internal_SetMargins(ScriptGUIElementStyle* nativeInstance, RectOffset* value)
	{
		nativeInstance->mElementStyle->margins = *value;
	}


	void ScriptGUIElementStyle::internal_GetContentOffset(ScriptGUIElementStyle* nativeInstance, RectOffset* value)
	{
		*value = nativeInstance->mElementStyle->contentOffset;
	}

	void ScriptGUIElementStyle::internal_SetContentOffset(ScriptGUIElementStyle* nativeInstance, RectOffset* value)
	{
		nativeInstance->mElementStyle->contentOffset = *value;
	}


	void ScriptGUIElementStyle::internal_GetWidth(ScriptGUIElementStyle* nativeInstance, UINT32* value)
	{
		*value = nativeInstance->mElementStyle->width;
	}

	void ScriptGUIElementStyle::internal_SetWidth(ScriptGUIElementStyle* nativeInstance, UINT32 value)
	{
		nativeInstance->mElementStyle->width = value;
	}


	void ScriptGUIElementStyle::internal_GetHeight(ScriptGUIElementStyle* nativeInstance, UINT32* value)
	{
		*value = nativeInstance->mElementStyle->height;
	}

	void ScriptGUIElementStyle::internal_SetHeight(ScriptGUIElementStyle* nativeInstance, UINT32 value)
	{
		nativeInstance->mElementStyle->height = value;
	}


	void ScriptGUIElementStyle::internal_GetMinWidth(ScriptGUIElementStyle* nativeInstance, UINT32* value)
	{
		*value = nativeInstance->mElementStyle->minWidth;
	}

	void ScriptGUIElementStyle::internal_SetMinWidth(ScriptGUIElementStyle* nativeInstance, UINT32 value)
	{
		nativeInstance->mElementStyle->minWidth = value;
	}


	void ScriptGUIElementStyle::internal_GetMaxWidth(ScriptGUIElementStyle* nativeInstance, UINT32* value)
	{
		*value = nativeInstance->mElementStyle->maxWidth;
	}

	void ScriptGUIElementStyle::internal_SetMaxWidth(ScriptGUIElementStyle* nativeInstance, UINT32 value)
	{
		nativeInstance->mElementStyle->maxWidth = value;
	}


	void ScriptGUIElementStyle::internal_GetMinHeight(ScriptGUIElementStyle* nativeInstance, UINT32* value)
	{
		*value = nativeInstance->mElementStyle->minHeight;
	}

	void ScriptGUIElementStyle::internal_SetMinHeight(ScriptGUIElementStyle* nativeInstance, UINT32 value)
	{
		nativeInstance->mElementStyle->minHeight = value;
	}


	void ScriptGUIElementStyle::internal_GetMaxHeight(ScriptGUIElementStyle* nativeInstance, UINT32* value)
	{
		*value = nativeInstance->mElementStyle->maxHeight;
	}

	void ScriptGUIElementStyle::internal_SetMaxHeight(ScriptGUIElementStyle* nativeInstance, UINT32 value)
	{
		nativeInstance->mElementStyle->maxHeight = value;
	}


	void ScriptGUIElementStyle::internal_GetFixedWidth(ScriptGUIElementStyle* nativeInstance, bool* value)
	{
		*value = nativeInstance->mElementStyle->fixedWidth;
	}

	void ScriptGUIElementStyle::internal_SetFixedWidth(ScriptGUIElementStyle* nativeInstance, bool value)
	{
		nativeInstance->mElementStyle->fixedWidth = value;
	}


	void ScriptGUIElementStyle::internal_GetFixedHeight(ScriptGUIElementStyle* nativeInstance, bool* value)
	{
		*value = nativeInstance->mElementStyle->fixedHeight;
	}

	void ScriptGUIElementStyle::internal_SetFixedHeight(ScriptGUIElementStyle* nativeInstance, bool value)
	{
		nativeInstance->mElementStyle->fixedHeight = value;
	}
}