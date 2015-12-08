#include "BsScriptResourceRef.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsResources.h"
#include "BsScriptResource.h"
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
	}

	MonoObject* ScriptResourceRef::create(UINT32 typeId)
	{
		MonoClass* resourceClass = ScriptResource::getClassFromTypeId(typeId);
		if (resourceClass == nullptr)
			return nullptr;

		MonoType* genParams[1] = { mono_class_get_type(resourceClass->_getInternalClass()) };

		::MonoClass* resourceRefClass = mono_class_bind_generic_parameters(metaData.scriptClass->_getInternalClass(), 1, genParams, false);
		MonoObject* obj = mono_object_new(MonoManager::instance().getDomain(), resourceRefClass);
		mono_runtime_object_init(obj);

		WeakResourceHandle<Resource> emptyHandle;
		ScriptResourceRef* scriptResourceRef = new (bs_alloc<ScriptResourceRef>()) ScriptResourceRef(obj, emptyHandle);

		return obj;
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
}

