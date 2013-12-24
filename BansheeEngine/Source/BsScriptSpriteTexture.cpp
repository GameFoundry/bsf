#include "BsScriptSpriteTexture.h"
#include "BsScriptMeta.h"
#include "BsScriptField.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
#include "BsSpriteTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptSpriteTexture::ScriptSpriteTexture(const SpriteTexturePtr& texture)
		:mTexture(texture)
	{

	}

	void ScriptSpriteTexture::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "SpriteTexture", &ScriptSpriteTexture::initRuntimeData);

		ScriptManager::registerScriptType(&metaData);
	}

	void ScriptSpriteTexture::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptSpriteTexture::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptSpriteTexture::internal_destroyInstance);
	}

	void ScriptSpriteTexture::internal_createInstance(MonoObject* instance)
	{
		ScriptSpriteTexture* nativeInstance = new (cm_alloc<ScriptSpriteTexture>()) ScriptSpriteTexture(nullptr); // TODO - DUMMY CODE!
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptSpriteTexture::internal_destroyInstance(ScriptSpriteTexture* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}