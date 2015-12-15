#include "BsScriptResourceRef.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsResources.h"
#include "BsScriptResource.h"
#include "BsScriptTexture2D.h"
#include "BsScriptTexture3D.h"
#include "BsScriptTextureCube.h"
#include "BsScriptResourceManager.h"

namespace BansheeEngine
{
	ScriptResourceRef::ScriptResourceRef(MonoObject* instance, const WeakResourceHandle<Resource>& resource)
		:ScriptObject(instance), mResource(resource)
	{ }

	void ScriptResourceRef::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_IsLoaded", &ScriptResourceRef::internal_IsLoaded);
		metaData.scriptClass->addInternalCall("Internal_GetResource", &ScriptResourceRef::internal_GetResource);
		metaData.scriptClass->addInternalCall("Internal_GetUUID", &ScriptResourceRef::internal_GetUUID);
	}

	MonoObject* ScriptResourceRef::createInternal(const WeakResourceHandle<Resource>& handle)
	{
		MonoObject* obj = metaData.scriptClass->createInstance();
		ScriptResourceRef* scriptResourceRef = new (bs_alloc<ScriptResourceRef>()) ScriptResourceRef(obj, handle);

		return obj;
	}

	MonoObject* ScriptResourceRef::create(const WeakResourceHandle<Texture>& handle, TextureType type)
	{
		switch (type)
		{
		case TEX_TYPE_2D:
			return createInternal(handle);
		case TEX_TYPE_3D:
			return createInternal(handle);
		case TEX_TYPE_CUBE_MAP:
			return createInternal(handle);
		}

		return nullptr;
	}

	bool ScriptResourceRef::internal_IsLoaded(ScriptResourceRef* nativeInstance)
	{
		return nativeInstance->mResource.isLoaded(false);
	}

	MonoObject* ScriptResourceRef::internal_GetResource(ScriptResourceRef* nativeInstance)
	{
		HResource resource = gResources().load(nativeInstance->mResource);
		ScriptResourceBase* scriptResource;

		ScriptResourceManager::instance().getScriptResource(resource, &scriptResource, true);

		return scriptResource->getManagedInstance();
	}

	MonoString* ScriptResourceRef::internal_GetUUID(ScriptResourceRef* thisPtr)
	{
		const String& uuid = thisPtr->getHandle().getUUID();

		return MonoUtil::stringToMono(uuid);
	}

}

