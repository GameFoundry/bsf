#include "BsScriptManagedResource.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsManagedResource.h"
#include "BsResources.h"
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
		
		// The only way this method should be reachable is when Resource::unload is called, which means the resource
		// has had to been already freed. Even if all managed instances are released ManagedResource itself holds the last
		// instance which is only freed on unload().
		// Note: During domain unload this could get called even if not all instances are released, but ManagedResourceManager
		// should make sure all instances are unloaded before that happens.
		assert(mResource == nullptr || !mResource.isLoaded()); 
		
		ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptManagedResource::setNativeHandle(const HResource& resource)
	{
		mResource = static_resource_cast<ManagedResource>(resource);
	}
}