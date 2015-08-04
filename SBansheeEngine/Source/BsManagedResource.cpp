#include "BsManagedResource.h"
#include "BsManagedResourceRTTI.h"
#include "BsManagedResourceMetaData.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsResources.h"
#include "BsManagedResourceManager.h"
#include "BsManagedSerializableObject.h"
#include "BsMemorySerializer.h"
#include "BsScriptResourceManager.h"
#include "BsMonoUtil.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	ManagedResource::ManagedResource()
		:Resource(false), mManagedInstance(nullptr)
	{ }

	ManagedResource::ManagedResource(MonoObject* managedInstance)
		:Resource(false), mManagedInstance(nullptr)
	{
		ManagedResourceMetaDataPtr metaData = bs_shared_ptr<ManagedResourceMetaData>();
		mMetaData = metaData;

		String elementNs;
		String elementTypeName;
		MonoUtil::getClassName(managedInstance, metaData->typeNamespace, metaData->typeName);

		MonoClass* managedClass = MonoManager::instance().findClass(metaData->typeNamespace, metaData->typeName);
		if (managedClass == nullptr)
		{
			LOGWRN("Cannot create managed component: " + metaData->typeNamespace + "." + metaData->typeName + " because that type doesn't exist.");
			return;
		}
	}

	ResourceBackupData ManagedResource::backup(bool clearExisting)
	{
		ManagedSerializableObjectPtr serializableObject = ManagedSerializableObject::createFromExisting(mManagedInstance);

		ResourceBackupData backupData;
		if (serializableObject != nullptr)
		{
			MemorySerializer ms;

			backupData.size = 0;
			backupData.data = ms.encode(serializableObject.get(), backupData.size);
		}
		else
		{
			backupData.size = 0;
			backupData.data = nullptr;
		}

		if (clearExisting)
		{
			if (mManagedInstance != nullptr)
			{
				mManagedInstance = nullptr;
				mono_gchandle_free(mManagedHandle);
				mManagedHandle = 0;
			}
		}

		return backupData;
	}

	void ManagedResource::restore(MonoObject* instance, const ResourceBackupData& data)
	{
		mManagedInstance = instance;

		if (mManagedInstance != nullptr)
		{
			mManagedHandle = mono_gchandle_new(mManagedInstance, false);

			if (data.data != nullptr)
			{
				MemorySerializer ms;
				ManagedSerializableObjectPtr serializableObject = std::static_pointer_cast<ManagedSerializableObject>(ms.decode(data.data, data.size));
				serializableObject->deserialize();
			}
		}
		else
		{
			// Could not restore resource
			ManagedResourceManager::instance().unregisterManagedResource(mMyHandle);
		}
	}

	HManagedResource ManagedResource::create(MonoObject* managedResource)
	{
		ManagedResourcePtr newRes = bs_core_ptr<ManagedResource, GenAlloc>(new (bs_alloc<ManagedResource>()) ManagedResource(managedResource));
		newRes->_setThisPtr(newRes);
		newRes->initialize();

		HManagedResource handle = static_resource_cast<ManagedResource>(gResources()._createResourceHandle(newRes));
		newRes->setHandle(managedResource, handle);

		return handle;
	}

	ManagedResourcePtr ManagedResource::createEmpty()
	{
		ManagedResourcePtr newRes = bs_core_ptr<ManagedResource, GenAlloc>(new (bs_alloc<ManagedResource>()) ManagedResource());
		newRes->_setThisPtr(newRes);
		newRes->initialize();

		return newRes;
	}

	void ManagedResource::setHandle(MonoObject* object, const HManagedResource& myHandle)
	{
		mManagedInstance = object;
		mManagedHandle = mono_gchandle_new(mManagedInstance, false);
		mMyHandle = myHandle;

		ScriptManagedResource* scriptInstance;
		ScriptResourceManager::instance().createScriptResource(object, myHandle, &scriptInstance);
		ManagedResourceManager::instance().registerManagedResource(mMyHandle);
	}

	void ManagedResource::destroy()
	{
		Resource::destroy();

		if (mManagedInstance != nullptr)
		{
			mManagedInstance = nullptr;
			mono_gchandle_free(mManagedHandle);
		}

		ManagedResourceManager::instance().unregisterManagedResource(mMyHandle);
	}

	RTTITypeBase* ManagedResource::getRTTIStatic()
	{
		return ManagedResourceRTTI::instance();
	}

	RTTITypeBase* ManagedResource::getRTTI() const
	{
		return ManagedResource::getRTTIStatic();
	}
}