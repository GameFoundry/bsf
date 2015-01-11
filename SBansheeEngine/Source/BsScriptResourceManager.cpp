#include "BsScriptResourceManager.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsScriptTexture2D.h"
#include "BsScriptTexture3D.h"
#include "BsScriptTextureCube.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptFont.h"
#include "BsScriptManagedResource.h"
#include "BsScriptAssemblyManager.h"

namespace BansheeEngine
{
	ScriptResourceManager::ScriptResourceManager()
	{
	}

	ScriptTexture2D* ScriptResourceManager::createScriptTexture2D(const HTexture& resourceHandle)
	{
		MonoClass* textureClass = ScriptAssemblyManager::instance().getTextureClass();
		MonoObject* monoInstance = textureClass->createInstance();

		return createScriptTexture2D(monoInstance, resourceHandle);
	}

	ScriptTexture3D* ScriptResourceManager::createScriptTexture3D(const HTexture& resourceHandle)
	{
		MonoClass* textureClass = ScriptAssemblyManager::instance().getTextureClass();
		MonoObject* monoInstance = textureClass->createInstance();

		return createScriptTexture3D(monoInstance, resourceHandle);
	}

	ScriptTextureCube* ScriptResourceManager::createScriptTextureCube(const HTexture& resourceHandle)
	{
		MonoClass* textureClass = ScriptAssemblyManager::instance().getTextureClass();
		MonoObject* monoInstance = textureClass->createInstance();

		return createScriptTextureCube(monoInstance, resourceHandle);
	}

	ScriptTexture2D* ScriptResourceManager::createScriptTexture2D(MonoObject* instance, const HTexture& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptTexture2D* scriptResource = new (bs_alloc<ScriptTexture2D>()) ScriptTexture2D(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptTexture3D* ScriptResourceManager::createScriptTexture3D(MonoObject* instance, const HTexture& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptTexture3D* scriptResource = new (bs_alloc<ScriptTexture3D>()) ScriptTexture3D(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptTextureCube* ScriptResourceManager::createScriptTextureCube(MonoObject* instance, const HTexture& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptTextureCube* scriptResource = new (bs_alloc<ScriptTextureCube>()) ScriptTextureCube(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptSpriteTexture* ScriptResourceManager::createScriptSpriteTexture(const HSpriteTexture& resourceHandle)
	{
		MonoClass* spriteTextureClass = ScriptAssemblyManager::instance().getSpriteTextureClass();
		MonoObject* monoInstance = spriteTextureClass->createInstance();

		return createScriptSpriteTexture(monoInstance, resourceHandle);
	}

	ScriptSpriteTexture* ScriptResourceManager::createScriptSpriteTexture(MonoObject* instance, const HSpriteTexture& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptSpriteTexture* scriptResource = new (bs_alloc<ScriptSpriteTexture>()) ScriptSpriteTexture(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptFont* ScriptResourceManager::createScriptFont(const HFont& resourceHandle)
	{
		MonoClass* fontClass = ScriptAssemblyManager::instance().getFontClass();
		MonoObject* monoInstance = fontClass->createInstance();

		return createScriptFont(monoInstance, resourceHandle);
	}

	ScriptFont* ScriptResourceManager::createScriptFont(MonoObject* instance, const HFont& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptFont* scriptResource = new (bs_alloc<ScriptFont>()) ScriptFont(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptManagedResource* ScriptResourceManager::createManagedResource(MonoObject* existingInstance, const HManagedResource& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
		throwExceptionIfInvalidOrDuplicate(uuid);

		ScriptManagedResource* scriptResource = new (bs_alloc<ScriptManagedResource>()) ScriptManagedResource(existingInstance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptTexture2D* ScriptResourceManager::getScriptTexture(const HTexture& resourceHandle)
	{
		return static_cast<ScriptTexture2D*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptSpriteTexture* ScriptResourceManager::getScriptSpriteTexture(const HSpriteTexture& resourceHandle)
	{
		return static_cast<ScriptSpriteTexture*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptManagedResource* ScriptResourceManager::getScriptManagedResource(const HManagedResource& resourceHandle)
	{
		return static_cast<ScriptManagedResource*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptResourceBase* ScriptResourceManager::getScriptResource(const String& uuid)
	{
		if(uuid == "")
			BS_EXCEPT(InvalidParametersException, "Provided resource handle has an undefined resource UUID.");

		auto findIter = mScriptResources.find(uuid);
		if(findIter != mScriptResources.end())
			return findIter->second;

		return nullptr;
	}

	ScriptResourceBase* ScriptResourceManager::createScriptResource(const HResource& resource)
	{
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(resource.getUUID());
#endif

		UINT32 resTypeID = resource->getTypeId();

		switch (resTypeID)
		{
		case TID_Texture:
			return createScriptTexture2D(static_resource_cast<Texture>(resource));
		case TID_SpriteTexture:
			return createScriptSpriteTexture(static_resource_cast<SpriteTexture>(resource));
		case TID_Font:
			return createScriptFont(static_resource_cast<Font>(resource));
		case TID_ManagedResource:
			BS_EXCEPT(InternalErrorException, "Managed resources must have a managed instance by default, this call is invalid.")
				break;
		default:
			BS_EXCEPT(NotImplementedException, "Attempting to load a resource type that is not supported. Type ID: " + toString(resTypeID));
			break;
		}
	}

	void ScriptResourceManager::destroyScriptResource(ScriptResourceBase* resource)
	{
		HResource resourceHandle = resource->getNativeHandle();
		const String& uuid = resourceHandle.getUUID();

		if(uuid == "")
			BS_EXCEPT(InvalidParametersException, "Provided resource handle has an undefined resource UUID.");

		auto findIter = mScriptResources.find(uuid);
		if(findIter != mScriptResources.end())
		{
			(resource)->~ScriptResourceBase();
			MemoryAllocator<GenAlloc>::free(resource);

			mScriptResources.erase(findIter);
		}
	}

	void ScriptResourceManager::throwExceptionIfInvalidOrDuplicate(const String& uuid) const
	{
		if(uuid == "")
			BS_EXCEPT(InvalidParametersException, "Provided resource handle has an undefined resource UUID.");

		auto findIter = mScriptResources.find(uuid);
		if(findIter != mScriptResources.end())
		{
			BS_EXCEPT(InvalidStateException, "Provided resource handle already has a script resource. \
											 Retrieve the existing instance instead of creating a new one.");
		}
	}
}