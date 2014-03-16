#include "BsScriptSpriteTexture.h"
#include "BsScriptResourceManager.h"
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
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "SpriteTexture", &ScriptSpriteTexture::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptSpriteTexture::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptSpriteTexture::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptSpriteTexture::internal_destroyInstance);
	}

	void ScriptSpriteTexture::internal_createInstance(MonoObject* instance)
	{
		ScriptResourceManager::instance().createScriptSpriteTexture(instance, SpriteTexture::dummy()); // TODO - DUMMY CODE!
	}

	void ScriptSpriteTexture::internal_destroyInstance(ScriptSpriteTexture* nativeInstance)
	{
		ScriptResourceManager::instance().destroyScriptResource(nativeInstance);
	}

	void ScriptSpriteTexture::setNativeHandle(const HResource& resource) 
	{ 
		mTexture = static_resource_cast<Resource>(mTexture); 
	}
}