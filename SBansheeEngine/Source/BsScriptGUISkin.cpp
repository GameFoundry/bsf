#include "BsScriptGUISkin.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsGUISkin.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGUIElementStyle.h"

namespace BansheeEngine
{
	ScriptGUISkin::ScriptGUISkin(MonoObject* instance, const HGUISkin& skin)
		:ScriptObject(instance), mSkin(skin)
	{ }

	void ScriptGUISkin::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUISkin::internal_CreateInstance);

		metaData.scriptClass->addInternalCall("Internal_GetStyle", &ScriptGUISkin::internal_GetStyle);
		metaData.scriptClass->addInternalCall("Internal_SetStyle", &ScriptGUISkin::internal_SetStyle);

		metaData.scriptClass->addInternalCall("Internal_RemoveStyle", &ScriptGUISkin::internal_RemoveStyle);
		metaData.scriptClass->addInternalCall("Internal_GetStyleNames", &ScriptGUISkin::internal_GetStyleNames);
	}

	void ScriptGUISkin::internal_CreateInstance(MonoObject* instance)
	{
		HGUISkin skin = GUISkin::create();

		ScriptGUISkin* scriptInstance;
		ScriptResourceManager::instance().createScriptResource(instance, skin, &scriptInstance);
	}

	MonoObject* ScriptGUISkin::internal_GetStyle(ScriptGUISkin* thisPtr, MonoString* name)
	{
		String nativeName = MonoUtil::monoToString(name);

		const GUIElementStyle* style = thisPtr->mSkin->getStyle(nativeName);
		if (style == nullptr)
			return nullptr;
		
		return ScriptGUIElementStyle::create(nativeName, *style);
	}

	void ScriptGUISkin::internal_SetStyle(ScriptGUISkin* thisPtr, MonoString* name, ScriptGUIElementStyle* style)
	{
		String nativeName = MonoUtil::monoToString(name);

		if (style != nullptr)
			thisPtr->mSkin->setStyle(nativeName, style->getInternalValue());
	}

	void ScriptGUISkin::internal_RemoveStyle(ScriptGUISkin* thisPtr, MonoString* name)
	{
		String nativeName = MonoUtil::monoToString(name);

		thisPtr->mSkin->removeStyle(nativeName);
	}

	MonoArray* ScriptGUISkin::internal_GetStyleNames(ScriptGUISkin* thisPtr)
	{
		Vector<String> styleNames = thisPtr->mSkin->getStyleNames();
		UINT32 numNames = (UINT32)styleNames.size();

		ScriptArray output = ScriptArray::create<String>(numNames);
		for (UINT32 i = 0; i < numNames; i++)
			output.set(i, styleNames[i]);

		return output.getInternal();
	}

	void ScriptGUISkin::setNativeHandle(const HResource& resource)
	{
		mSkin = static_resource_cast<GUISkin>(resource);
	}

	void ScriptGUISkin::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}
}