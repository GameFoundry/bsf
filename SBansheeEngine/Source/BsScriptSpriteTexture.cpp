#include "BsScriptSpriteTexture.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"

namespace BansheeEngine
{
	ScriptSpriteTexture::ScriptSpriteTexture(MonoObject* instance,const HSpriteTexture& texture)
		:ScriptObject(instance), mTexture(texture)
	{

	}

	void ScriptSpriteTexture::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptSpriteTexture::internal_createInstance);
	}

	void ScriptSpriteTexture::internal_createInstance(MonoObject* instance)
	{
		ScriptResourceManager::instance().createScriptSpriteTexture(instance, SpriteTexture::dummy()); // TODO - DUMMY CODE!
	}

	void ScriptSpriteTexture::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;
		ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptSpriteTexture::setNativeHandle(const HResource& resource) 
	{ 
		mTexture = static_resource_cast<Resource>(mTexture); 
	}
}