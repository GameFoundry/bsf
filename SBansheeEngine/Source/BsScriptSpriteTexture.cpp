#include "BsScriptSpriteTexture.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptSpriteTexture::ScriptSpriteTexture(const HSpriteTexture& texture)
		:mTexture(texture)
	{

	}

	void ScriptSpriteTexture::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "SpriteTexture", &ScriptSpriteTexture::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptSpriteTexture::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptSpriteTexture::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptSpriteTexture::internal_destroyInstance);
	}

	void ScriptSpriteTexture::internal_createInstance(MonoObject* instance)
	{
		ScriptSpriteTexture* nativeInstance = new (cm_alloc<ScriptSpriteTexture>()) ScriptSpriteTexture(SpriteTexture::dummy()); // TODO - DUMMY CODE!
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptSpriteTexture::internal_destroyInstance(ScriptSpriteTexture* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}