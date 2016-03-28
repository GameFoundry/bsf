//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsResourceRTTI.h"
#include "BsManagedResource.h"
#include "BsMonoManager.h"
#include "BsManagedSerializableObject.h"
#include "BsResources.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-SEngine
	 *  @{
	 */

	class BS_SCR_BE_EXPORT ManagedResourceRTTI : public RTTIType<ManagedResource, Resource, ManagedResourceRTTI>
	{
	private:
		ManagedSerializableObjectPtr getObjectData(ManagedResource* obj)
		{
			return any_cast<ManagedSerializableObjectPtr>(obj->mRTTIData);
		}

		void setObjectData(ManagedResource* obj, ManagedSerializableObjectPtr val)
		{
			obj->mRTTIData = val;
		}

	public:
		ManagedResourceRTTI()
		{
			addReflectablePtrField("mObjectData", 0, &ManagedResourceRTTI::getObjectData, &ManagedResourceRTTI::setObjectData);
		}

		void onSerializationStarted(IReflectable* obj) override
		{
			ManagedResource* mc = static_cast<ManagedResource*>(obj);

			mc->mRTTIData = ManagedSerializableObject::createFromExisting(mc->getManagedInstance());
		}

		void onDeserializationEnded(IReflectable* obj) override
		{
			ManagedResource* mr = static_cast<ManagedResource*>(obj);
			ManagedSerializableObjectPtr serializableObject = any_cast<ManagedSerializableObjectPtr>(mr->mRTTIData);
			serializableObject->deserialize();

			ResourcePtr mrPtr = std::static_pointer_cast<Resource>(mr->getThisPtr());
			HManagedResource handle = static_resource_cast<ManagedResource>(gResources()._createResourceHandle(mrPtr));
			mr->setHandle(serializableObject->getManagedInstance(), handle);
			mr->mRTTIData = nullptr;
		}

		const String& getRTTIName() override
		{
			static String name = "ManagedResource";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ManagedResource;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return ManagedResource::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}