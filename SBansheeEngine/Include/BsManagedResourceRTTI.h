#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsResourceRTTI.h"
#include "BsManagedResource.h"
#include "BsMonoManager.h"
#include "BsManagedSerializableObject.h"

namespace BansheeEngine
{
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

		void onSerializationStarted(IReflectable* obj)
		{
			ManagedResource* mc = static_cast<ManagedResource*>(obj);

			mc->mRTTIData = ManagedSerializableObject::createFromExisting(mc->getManagedInstance());
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			ManagedResource* mc = static_cast<ManagedResource*>(obj);
			ManagedSerializableObjectPtr serializableObject = any_cast<ManagedSerializableObjectPtr>(mc->mRTTIData);

			mc->construct(serializableObject->getManagedInstance());
		}

		virtual const String& getRTTIName()
		{
			static String name = "ManagedResource";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ManagedResource;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ManagedResource::createEmpty();
		}
	};
}