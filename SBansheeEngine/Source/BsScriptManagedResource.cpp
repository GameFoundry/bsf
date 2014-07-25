#include "BsScriptManagedResource.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsManagedResource.h"
#include "BsException.h"

namespace BansheeEngine
{
	ScriptManagedResource::ScriptManagedResource(MonoObject* instance, const HManagedResource& resource)
		:ScriptObject(instance), mResource(resource)
	{

	}

	void ScriptManagedResource::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptManagedResource::internal_createInstance);
	}

	void ScriptManagedResource::internal_createInstance(MonoObject* instance)
	{
		HManagedResource resource = ManagedResource::create(instance);

		ScriptResourceManager::instance().createManagedResource(instance, resource);
	}

	void ScriptManagedResource::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;
		ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptManagedResource::setNativeHandle(const HResource& resource)
	{
		mResource = static_resource_cast<ManagedResource>(resource);
	}
}