#include "BsScriptResourceManager.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsScriptTexture2D.h"
#include "BsScriptSpriteTexture.h"

namespace BansheeEngine
{
	ScriptResourceManager::ScriptResourceManager()
		:mTextureClass(nullptr), mSpriteTextureClass(nullptr)
	{
		MonoAssembly* assembly = MonoManager::instance().getAssembly(BansheeEngineAssemblyName);
		if(assembly == nullptr)
			BS_EXCEPT(InternalErrorException, "Cannot find \"" + String(BansheeEngineAssemblyName) + "\" assembly.");

		mTextureClass = assembly->getClass("BansheeEngine", "Texture2D");
		mSpriteTextureClass = assembly->getClass("BansheeEngine", "SpriteTexture");

		if(mTextureClass == nullptr)
			BS_EXCEPT(InternalErrorException, "Cannot find managed Texture2D class.");

		if(mSpriteTextureClass == nullptr)
			BS_EXCEPT(InternalErrorException, "Cannot find managed SpriteTexture class.");
	}

	ScriptTexture2D* ScriptResourceManager::createScriptTexture(const HTexture& resourceHandle)
	{
		MonoObject* monoInstance = mTextureClass->createInstance();

		return createScriptTexture(monoInstance, resourceHandle);
	}

	ScriptTexture2D* ScriptResourceManager::createScriptTexture(MonoObject* instance, const HTexture& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
		throwExceptionIfInvalidOrDuplicate(uuid);

		ScriptTexture2D* scriptResource = new (bs_alloc<ScriptTexture2D>()) ScriptTexture2D(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptSpriteTexture* ScriptResourceManager::createScriptSpriteTexture(const HSpriteTexture& resourceHandle)
	{
		MonoObject* monoInstance = mSpriteTextureClass->createInstance();

		return createScriptSpriteTexture(monoInstance, resourceHandle);
	}

	ScriptSpriteTexture* ScriptResourceManager::createScriptSpriteTexture(MonoObject* instance, const HSpriteTexture& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
		throwExceptionIfInvalidOrDuplicate(uuid);

		ScriptSpriteTexture* scriptResource = new (bs_alloc<ScriptSpriteTexture>()) ScriptSpriteTexture(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptTexture2D* ScriptResourceManager::getScriptTexture(const HTexture& resourceHandle)
	{
		return static_cast<ScriptTexture2D*>(getScriptResource(resourceHandle));
	}

	ScriptSpriteTexture* ScriptResourceManager::getScriptSpriteTexture(const HSpriteTexture& resourceHandle)
	{
		return static_cast<ScriptSpriteTexture*>(getScriptResource(resourceHandle));
	}

	ScriptResourceBase* ScriptResourceManager::getScriptResource(const HResource& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();

		if(uuid == "")
			BS_EXCEPT(InvalidParametersException, "Provided resource handle has an undefined resource UUID.");

		auto findIter = mScriptResources.find(uuid);
		if(findIter != mScriptResources.end())
			return findIter->second;

		return nullptr;
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