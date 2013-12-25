#include "BsScriptGUIElementStateStyle.h"
#include "BsScriptMeta.h"
#include "BsScriptField.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
#include "BsScriptSpriteTexture.h"
#include "CmException.h"
#include "BsGUIElementStyle.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIElementStateStyle::ScriptGUIElementStateStyle()
		:mElementStateStyle(cm_new<GUIElementStyle::GUIElementStateStyle>()), mSpriteTexture(nullptr), mOwnsStyle(true)
	{

	}

	ScriptGUIElementStateStyle::ScriptGUIElementStateStyle(GUIElementStyle::GUIElementStateStyle* externalStyle)
		: mElementStateStyle(externalStyle), mSpriteTexture(nullptr), mOwnsStyle(false)
	{

	}

	ScriptGUIElementStateStyle::~ScriptGUIElementStateStyle()
	{
		if(mOwnsStyle)
			cm_delete(mElementStateStyle);
	}

	void ScriptGUIElementStateStyle::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIElementStateStyle", &ScriptGUIElementStateStyle::initRuntimeData);

		ScriptManager::registerScriptType(&metaData);
	}

	void ScriptGUIElementStateStyle::internal_createInstance(MonoObject* instance)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle();
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIElementStateStyle::internal_createInstanceExternal(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStyle)
	{
		ScriptGUIElementStateStyle* nativeInstance = new (cm_alloc<ScriptGUIElementStateStyle>()) ScriptGUIElementStateStyle(externalStyle);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIElementStateStyle::internal_destroyInstance(ScriptGUIElementStateStyle* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}