#include "BsManagedResource.h"
#include "BsManagedResourceRTTI.h"
#include "BsManagedResourceMetaData.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsResources.h"
#include "BsManagedResourceManager.h"
#include "BsManagedSerializableField.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsManagedSerializableObjectData.h"
#include "BsMemorySerializer.h"
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

		::MonoClass* monoClass = mono_object_get_class(managedInstance);

		metaData->typeNamespace = mono_class_get_namespace(monoClass);
		metaData->typeName = mono_class_get_name(monoClass);

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
			ManagedSerializableObjectInfoPtr objectInfo = serializableObject->getObjectInfo();
			ManagedSerializableObjectDataPtr objectData = serializableObject->getObjectData();

			MemorySerializer ms;

			backupData.mTypeInfo.size = 0;
			backupData.mTypeInfo.data = ms.encode(objectInfo.get(), backupData.mTypeInfo.size);

			backupData.mObjectData.size = 0;
			backupData.mObjectData.data = ms.encode(objectData.get(), backupData.mObjectData.size);
		}
		else
		{
			backupData.mTypeInfo.size = 0;
			backupData.mTypeInfo.data = nullptr;

			backupData.mObjectData.size = 0;
			backupData.mObjectData.data = nullptr;
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

			if (data.mTypeInfo.data != nullptr && data.mObjectData.data != nullptr)
			{
				MemorySerializer ms;
				ManagedSerializableObjectInfoPtr objectInfo = std::static_pointer_cast<ManagedSerializableObjectInfo>(ms.decode(data.mTypeInfo.data, data.mTypeInfo.size));
				ManagedSerializableObjectDataPtr objectData = std::static_pointer_cast<ManagedSerializableObjectData>(ms.decode(data.mObjectData.data, data.mObjectData.size));

				ManagedSerializableObjectPtr serializableObject = ManagedSerializableObject::createFromExisting(instance);
				serializableObject->setObjectData(objectData, objectInfo);
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
		newRes->construct(managedResource, handle);

		return handle;
	}

	ManagedResourcePtr ManagedResource::createEmpty()
	{
		ManagedResourcePtr newRes = bs_core_ptr<ManagedResource, GenAlloc>(new (bs_alloc<ManagedResource>()) ManagedResource());
		newRes->_setThisPtr(newRes);
		newRes->initialize();

		return newRes;
	}

	void ManagedResource::construct(MonoObject* object, const HManagedResource& myHandle)
	{
		mManagedInstance = object;
		mManagedHandle = mono_gchandle_new(mManagedInstance, false);
		mMyHandle = myHandle;

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