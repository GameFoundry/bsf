#include "BsScriptResourceManager.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsScriptTexture2D.h"
#include "BsScriptTexture3D.h"
#include "BsScriptTextureCube.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptPlainText.h"
#include "BsScriptScriptCode.h"
#include "BsScriptShader.h"
#include "BsScriptMaterial.h"
#include "BsScriptMesh.h"
#include "BsScriptFont.h"
#include "BsScriptPrefab.h"
#include "BsScriptManagedResource.h"
#include "BsScriptAssemblyManager.h"

namespace BansheeEngine
{
	ScriptResourceManager::ScriptResourceManager()
	{
	}

	ScriptTexture2D* ScriptResourceManager::createScriptTexture2D(const HTexture& resourceHandle)
	{
		MonoClass* textureClass = ScriptAssemblyManager::instance().getTexture2DClass();
		MonoObject* monoInstance = textureClass->createInstance();

		return createScriptTexture2D(monoInstance, resourceHandle);
	}

	ScriptTexture3D* ScriptResourceManager::createScriptTexture3D(const HTexture& resourceHandle)
	{
		MonoClass* textureClass = ScriptAssemblyManager::instance().getTexture3DClass();
		MonoObject* monoInstance = textureClass->createInstance();

		return createScriptTexture3D(monoInstance, resourceHandle);
	}

	ScriptTextureCube* ScriptResourceManager::createScriptTextureCube(const HTexture& resourceHandle)
	{
		MonoClass* textureClass = ScriptAssemblyManager::instance().getTextureCubeClass();
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

	ScriptShader* ScriptResourceManager::createScriptShader(const HShader& resourceHandle)
	{
		MonoClass* shaderClass = ScriptAssemblyManager::instance().getShaderClass();
		MonoObject* monoInstance = shaderClass->createInstance();

		return createScriptShader(monoInstance, resourceHandle);
	}

	ScriptShader* ScriptResourceManager::createScriptShader(MonoObject* instance, const HShader& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptShader* scriptResource = new (bs_alloc<ScriptShader>()) ScriptShader(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptMaterial* ScriptResourceManager::createScriptMaterial(const HMaterial& resourceHandle)
	{
		MonoClass* materialClass = ScriptAssemblyManager::instance().getMaterialClass();
		MonoObject* monoInstance = materialClass->createInstance();

		return createScriptMaterial(monoInstance, resourceHandle);
	}

	ScriptMaterial* ScriptResourceManager::createScriptMaterial(MonoObject* instance, const HMaterial& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptMaterial* scriptResource = new (bs_alloc<ScriptMaterial>()) ScriptMaterial(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptMesh* ScriptResourceManager::createScriptMesh(const HMesh& resourceHandle)
	{
		MonoClass* meshClass = ScriptAssemblyManager::instance().getMeshClass();
		MonoObject* monoInstance = meshClass->createInstance();

		return createScriptMesh(monoInstance, resourceHandle);
	}

	ScriptMesh* ScriptResourceManager::createScriptMesh(MonoObject* instance, const HMesh& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptMesh* scriptResource = new (bs_alloc<ScriptMesh>()) ScriptMesh(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptPlainText* ScriptResourceManager::createScriptPlainText(const HPlainText& resourceHandle)
	{
		MonoClass* plainTextClass = ScriptAssemblyManager::instance().getPlainTextClass();
		MonoObject* monoInstance = plainTextClass->createInstance();

		return createScriptPlainText(monoInstance, resourceHandle);
	}

	ScriptPlainText* ScriptResourceManager::createScriptPlainText(MonoObject* instance, const HPlainText& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptPlainText* scriptResource = new (bs_alloc<ScriptPlainText>()) ScriptPlainText(instance, resourceHandle);
		mScriptResources[uuid] = scriptResource;

		return scriptResource;
	}

	ScriptScriptCode* ScriptResourceManager::createScriptScriptCode(const HScriptCode& resourceHandle)
	{
		MonoClass* scriptCodeClass = ScriptAssemblyManager::instance().getScriptCodeClass();
		MonoObject* monoInstance = scriptCodeClass->createInstance();

		return createScriptScriptCode(monoInstance, resourceHandle);
	}

	ScriptScriptCode* ScriptResourceManager::createScriptScriptCode(MonoObject* instance, const HScriptCode& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptScriptCode* scriptResource = new (bs_alloc<ScriptScriptCode>()) ScriptScriptCode(instance, resourceHandle);
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

	ScriptPrefab* ScriptResourceManager::createScriptPrefab(const HPrefab& resourceHandle)
	{
		MonoClass* prefabClass = ScriptAssemblyManager::instance().getPrefabClass();
		MonoObject* monoInstance = prefabClass->createInstance();

		return createScriptPrefab(monoInstance, resourceHandle);
	}

	ScriptPrefab* ScriptResourceManager::createScriptPrefab(MonoObject* instance, const HPrefab& resourceHandle)
	{
		const String& uuid = resourceHandle.getUUID();
#if BS_DEBUG_MODE
		throwExceptionIfInvalidOrDuplicate(uuid);
#endif

		ScriptPrefab* scriptResource = new (bs_alloc<ScriptPrefab>()) ScriptPrefab(instance, resourceHandle);
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

	ScriptTexture2D* ScriptResourceManager::getScriptTexture2D(const HTexture& resourceHandle)
	{
		return static_cast<ScriptTexture2D*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptTexture3D* ScriptResourceManager::getScriptTexture3D(const HTexture& resourceHandle)
	{
		return static_cast<ScriptTexture3D*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptTextureCube* ScriptResourceManager::getScriptTextureCube(const HTexture& resourceHandle)
	{
		return static_cast<ScriptTextureCube*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptSpriteTexture* ScriptResourceManager::getScriptSpriteTexture(const HSpriteTexture& resourceHandle)
	{
		return static_cast<ScriptSpriteTexture*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptShader* ScriptResourceManager::getScriptShader(const HShader& resourceHandle)
	{
		return static_cast<ScriptShader*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptMaterial* ScriptResourceManager::getScriptMaterial(const HMaterial& resourceHandle)
	{
		return static_cast<ScriptMaterial*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptMesh* ScriptResourceManager::getScriptMesh(const HMesh& resourceHandle)
	{
		return static_cast<ScriptMesh*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptPlainText* ScriptResourceManager::getScriptPlainText(const HPlainText& resourceHandle)
	{
		return static_cast<ScriptPlainText*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptScriptCode* ScriptResourceManager::getScriptScriptCode(const HScriptCode& resourceHandle)
	{
		return static_cast<ScriptScriptCode*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptManagedResource* ScriptResourceManager::getScriptManagedResource(const HManagedResource& resourceHandle)
	{
		return static_cast<ScriptManagedResource*>(getScriptResource(resourceHandle.getUUID()));
	}

	ScriptPrefab* ScriptResourceManager::getScriptPrefab(const HPrefab& resourceHandle)
	{
		return static_cast<ScriptPrefab*>(getScriptResource(resourceHandle.getUUID()));
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
		{
			HTexture texture = static_resource_cast<Texture>(resource);
			TextureType type = texture->getProperties().getTextureType();

			if (type == TEX_TYPE_3D)
				return createScriptTexture3D(texture);
			else if (type == TEX_TYPE_CUBE_MAP)
				return createScriptTextureCube(texture);
			else
				return createScriptTexture2D(texture);
		}
		case TID_SpriteTexture:
			return createScriptSpriteTexture(static_resource_cast<SpriteTexture>(resource));
		case TID_Font:
			return createScriptFont(static_resource_cast<Font>(resource));
		case TID_PlainText:
			return createScriptPlainText(static_resource_cast<PlainText>(resource));
		case TID_ScriptCode:
			return createScriptScriptCode(static_resource_cast<ScriptCode>(resource));
		case TID_Shader:
			return createScriptShader(static_resource_cast<Shader>(resource));
		case TID_Material:
			return createScriptMaterial(static_resource_cast<Material>(resource));
		case TID_Mesh:
			return createScriptMesh(static_resource_cast<Mesh>(resource));
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