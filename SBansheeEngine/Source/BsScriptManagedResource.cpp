#include "BsScriptManagedResource.h"
#include "BsScriptResourceManager.h"
#include "BsScriptAssemblyManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsManagedResource.h"
#include "BsResources.h"
#include "BsException.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptManagedResource::ScriptManagedResource(MonoObject* instance, const HManagedResource& resource)
		:ScriptObject(instance), mResource(resource)
	{
		assert(instance != nullptr);

		MonoUtil::getClassName(instance, mNamespace, mType);
	}

	void ScriptManagedResource::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptManagedResource::internal_createInstance);
	}

	void ScriptManagedResource::internal_createInstance(MonoObject* instance)
	{
		HManagedResource resource = ManagedResource::create(instance);
	}

	MonoObject* ScriptManagedResource::_createManagedInstance(bool construct)
	{
		ManagedSerializableObjectInfoPtr currentObjInfo = nullptr;

		// See if this type even still exists
		if (!ScriptAssemblyManager::instance().getSerializableObjectInfo(mNamespace, mType, currentObjInfo))
			return nullptr;

		return currentObjInfo->mMonoClass->createInstance(construct);
	}

	ScriptObjectBackup ScriptManagedResource::beginRefresh()
	{
		ScriptResourceBase::beginRefresh();

		ScriptObjectBackup backupData;
		backupData.data = mResource->backup(true);

		return backupData;
	}

	void ScriptManagedResource::endRefresh(const ScriptObjectBackup& backupData)
	{
		ResourceBackupData resourceBackup = any_cast<ResourceBackupData>(backupData.data);
		mResource->restore(mManagedInstance, resourceBackup);

		// If we could not find resource type after refresh, treat it as if it was destroyed
		if (mManagedInstance == nullptr)
			_onManagedInstanceDeleted();

		ScriptResourceBase::endRefresh(backupData);
	}

	void ScriptManagedResource::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;
		
		if (!mRefreshInProgress)
		{
			// The only way this method should be reachable is when Resource::unload is called, which means the resource
			// has had to been already freed. Even if all managed instances are released ManagedResource itself holds the last
			// instance which is only freed on unload().
			// Note: During domain unload this could get called even if not all instances are released, but ManagedResourceManager
			// should make sure all instances are unloaded before that happens.
			assert(mResource == nullptr || !mResource.isLoaded());

			ScriptResourceManager::instance().destroyScriptResource(this);
		}
	}

	void ScriptManagedResource::setResource(const HResource& resource)
	{
		mResource = static_resource_cast<ManagedResource>(resource);
	}
}