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
	ScriptResourceRefBase::ScriptResourceRefBase(MonoObject* instance, const WeakResourceHandle<Resource>& resource)
		:ScriptObject(instance), mResource(resource)
	{ }

	void ScriptResourceRefBase::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_IsLoaded", &ScriptResourceRefBase::internal_IsLoaded);
		metaData.scriptClass->addInternalCall("Internal_GetResource", &ScriptResourceRefBase::internal_GetResource);
		metaData.scriptClass->addInternalCall("Internal_GetUUID", &ScriptResourceRefBase::internal_GetUUID);
	}

	bool ScriptResourceRefBase::internal_IsLoaded(ScriptResourceRefBase* nativeInstance)
	{
		return nativeInstance->mResource.isLoaded(false);
	}

	MonoObject* ScriptResourceRefBase::internal_GetResource(ScriptResourceRefBase* nativeInstance)
	{
		HResource resource = gResources().load(nativeInstance->mResource);
		ScriptResourceBase* scriptResource;

		ScriptResourceManager::instance().getScriptResource(resource, &scriptResource, true);

		return scriptResource->getManagedInstance();
	}

	MonoString* ScriptResourceRefBase::internal_GetUUID(ScriptResourceRefBase* thisPtr)
	{
		const String& uuid = thisPtr->getHandle().getUUID();

		return MonoUtil::stringToMono(uuid);
	}

	ScriptResourceRef::ScriptResourceRef(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptResourceRef::initRuntimeData()
	{ }

	MonoObject* ScriptResourceRef::create(::MonoClass* resourceClass, const WeakResourceHandle<Resource>& handle)
	{
		MonoType* genParams[1] = { mono_class_get_type(resourceClass) };

		::MonoClass* resourceRefClass = mono_class_bind_generic_parameters(metaData.scriptClass->_getInternalClass(), 1, genParams, false);
		MonoObject* obj = mono_object_new(MonoManager::instance().getDomain(), resourceRefClass);
		mono_runtime_object_init(obj);

		ScriptResourceRefBase* scriptResourceRef = new (bs_alloc<ScriptResourceRefBase>()) ScriptResourceRefBase(obj, handle);

		return obj;
	}

	MonoObject* ScriptResourceRef::create(const WeakResourceHandle<Texture>& handle, TextureType type)
	{
		switch (type)
		{
		case TEX_TYPE_2D:
			return create(ScriptTexture2D::getMetaData()->scriptClass->_getInternalClass(), handle);
		case TEX_TYPE_3D:
			return create(ScriptTexture3D::getMetaData()->scriptClass->_getInternalClass(), handle);
		case TEX_TYPE_CUBE_MAP:
			return create(ScriptTextureCube::getMetaData()->scriptClass->_getInternalClass(), handle);
		}

		return nullptr;

	}
}

