#include "BsScriptGUIElementStyle.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
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
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUIElementStyle", &ScriptGUIElementStyle::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
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
		cm_delete(nativeInstance);
	}
}