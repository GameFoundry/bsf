#include "BsManagedResource.h"
#include "BsManagedResourceRTTI.h"
#include "BsManagedResourceMetaData.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsResources.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	ManagedResource::ManagedResource()
		:mManagedInstance(nullptr)
	{ }

	ManagedResource::ManagedResource(MonoObject* managedInstance)
		:mManagedInstance(nullptr)
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

		construct(managedInstance);
	}

	HManagedResource ManagedResource::create(MonoObject* managedResource)
	{
		ManagedResourcePtr newRes = bs_core_ptr<ManagedResource, GenAlloc>(new (bs_alloc<ManagedResource>()) ManagedResource(managedResource));
		newRes->_setThisPtr(newRes);
		newRes->initialize();

		return static_resource_cast<ManagedResource>(gResources()._createResourceHandle(newRes));
	}

	ManagedResourcePtr ManagedResource::createEmpty()
	{
		ManagedResourcePtr newRes = bs_core_ptr<ManagedResource, GenAlloc>(new (bs_alloc<ManagedResource>()) ManagedResource());
		newRes->_setThisPtr(newRes);
		newRes->initialize();

		return newRes;
	}

	void ManagedResource::construct(MonoObject* object)
	{
		mManagedInstance = object;
		mManagedHandle = mono_gchandle_new(mManagedInstance, false);
	}

	void ManagedResource::destroy_internal()
	{
		Resource::destroy_internal();

		if (mManagedInstance != nullptr)
		{
			mManagedInstance = nullptr;
			mono_gchandle_free(mManagedHandle);
		}
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