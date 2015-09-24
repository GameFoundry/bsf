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
		metaData.scriptClass->addInternalCall("Internal_GetTexture", &ScriptSpriteTexture::internal_GetTexture);
		metaData.scriptClass->addInternalCall("Internal_SetTexture", &ScriptSpriteTexture::internal_SetTexture);
		metaData.scriptClass->addInternalCall("Internal_GetOffset", &ScriptSpriteTexture::internal_GetOffset);
		metaData.scriptClass->addInternalCall("Internal_SetOffset", &ScriptSpriteTexture::internal_SetOffset);
		metaData.scriptClass->addInternalCall("Internal_GetScale", &ScriptSpriteTexture::internal_GetScale);
		metaData.scriptClass->addInternalCall("Internal_SetScale", &ScriptSpriteTexture::internal_SetScale);
		metaData.scriptClass->addInternalCall("Internal_GetWidth", &ScriptSpriteTexture::internal_GetWidth);
		metaData.scriptClass->addInternalCall("Internal_GetHeight", &ScriptSpriteTexture::internal_GetHeight);
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

	MonoObject* ScriptSpriteTexture::internal_GetTexture(ScriptSpriteTexture* thisPtr)
	{
		HSpriteTexture spriteTexture = thisPtr->mTexture;
		if (!spriteTexture.isLoaded())
			return nullptr;

		HTexture texture = spriteTexture->getTexture();
		if (!texture.isLoaded())
			return nullptr;

		ScriptTexture2D* scriptTexture = nullptr;
		ScriptResourceManager::instance().getScriptResource(texture, &scriptTexture, true);

		return scriptTexture->getManagedInstance();
	}

	void ScriptSpriteTexture::internal_SetTexture(ScriptSpriteTexture* thisPtr, ScriptTexture2D* value)
	{
		HSpriteTexture spriteTexture = thisPtr->mTexture;
		if (!spriteTexture.isLoaded())
			return;

		HTexture texture;
		if (value != nullptr)
			texture = value->getTextureHandle();

		spriteTexture->setTexture(texture);
	}

	void ScriptSpriteTexture::internal_GetOffset(ScriptSpriteTexture* thisPtr, Vector2* value)
	{
		HSpriteTexture spriteTexture = thisPtr->mTexture;
		if (!spriteTexture.isLoaded())
		{
			*value = Vector2::ZERO;
			return;
		}
		
		*value = spriteTexture->getOffset();
	}

	void ScriptSpriteTexture::internal_SetOffset(ScriptSpriteTexture* thisPtr, Vector2 value)
	{
		HSpriteTexture spriteTexture = thisPtr->mTexture;
		if (!spriteTexture.isLoaded())
			return;

		spriteTexture->setOffset(value);
	}

	void ScriptSpriteTexture::internal_GetScale(ScriptSpriteTexture* thisPtr, Vector2* value)
	{
		HSpriteTexture spriteTexture = thisPtr->mTexture;
		if (!spriteTexture.isLoaded())
		{
			*value = Vector2::ZERO;
			return;
		}

		*value = spriteTexture->getScale();
	}

	void ScriptSpriteTexture::internal_SetScale(ScriptSpriteTexture* thisPtr, Vector2 value)
	{
		HSpriteTexture spriteTexture = thisPtr->mTexture;
		if (!spriteTexture.isLoaded())
			return;

		spriteTexture->setScale(value);
	}

	UINT32 ScriptSpriteTexture::internal_GetWidth(ScriptSpriteTexture* thisPtr)
	{
		HSpriteTexture spriteTexture = thisPtr->mTexture;
		if (!spriteTexture.isLoaded())
			return 0;

		return spriteTexture->getWidth();
	}

	UINT32 ScriptSpriteTexture::internal_GetHeight(ScriptSpriteTexture* thisPtr)
	{
		HSpriteTexture spriteTexture = thisPtr->mTexture;
		if (!spriteTexture.isLoaded())
			return 0;

		return spriteTexture->getHeight();
	}
}