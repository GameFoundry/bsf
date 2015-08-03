#include "BsScriptSpriteTexture.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsScriptTexture2D.h"

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

	void ScriptSpriteTexture::internal_createInstance(MonoObject* instance, MonoObject* texture, Vector2 offset, Vector2 scale)
	{
		ScriptTexture2D* scriptTexture = ScriptTexture2D::toNative(texture);
		ScriptSpriteTexture* scriptInstance;

		if (scriptTexture == nullptr)
		{
			ScriptResourceManager::instance().createScriptResource(instance, SpriteTexture::dummy(), &scriptInstance);
		}
		else
		{
			HSpriteTexture spriteTexture = SpriteTexture::create(offset, scale, static_resource_cast<Texture>(scriptTexture->getNativeHandle()));

			ScriptResourceManager::instance().createScriptResource(instance, spriteTexture, &scriptInstance);
		}
	}

	void ScriptSpriteTexture::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptSpriteTexture::setNativeHandle(const HResource& resource) 
	{ 
		mTexture = static_resource_cast<SpriteTexture>(resource);
	}

	MonoObject* ScriptSpriteTexture::toManaged(const HSpriteTexture& texture)
	{
		if (texture == nullptr)
			return nullptr;

		ScriptSpriteTexture* scriptSpriteTex;
		ScriptResourceManager::instance().getScriptResource(texture, &scriptSpriteTex, true);

		return scriptSpriteTex->getManagedInstance();
	}
}