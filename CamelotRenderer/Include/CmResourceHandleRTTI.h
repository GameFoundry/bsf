#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmResourceHandle.h"
#include "CmResources.h"

namespace CamelotEngine
{
	class CM_EXPORT ResourceHandleDataRTTI : public RTTIType<ResourceHandleData, IReflectable, ResourceHandleDataRTTI>
	{
	private:
		String& getUUID(ResourceHandleData* obj) { return obj->mUUID; }
		void setUUID(ResourceHandleData* obj, String& uuid) 
		{ 
			obj->mUUID = uuid; 
		} 
	public:
		ResourceHandleDataRTTI()
		{
			addPlainField("mUUID", 0, &ResourceHandleDataRTTI::getUUID, &ResourceHandleDataRTTI::setUUID);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ResourceHandleData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ResourceHandleData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<ResourceHandleData>(new ResourceHandleData());
		}
	};

	class CM_EXPORT ResourceHandleRTTI : public RTTIType<ResourceHandleBase, IReflectable, ResourceHandleRTTI>
	{
	private:
		std::shared_ptr<ResourceHandleData> getResData(ResourceHandleBase* obj) { return obj->mData; }
		void setResData(ResourceHandleBase* obj, std::shared_ptr<ResourceHandleData> val) { obj->mData = val; }
	public:
		ResourceHandleRTTI()
		{
			addReflectablePtrField("mData", 0, &ResourceHandleRTTI::getResData, &ResourceHandleRTTI::setResData);
		}

		void onDeserializationEnded(IReflectable* obj)
		{
			ResourceHandleBase* resourceHandle = static_cast<ResourceHandleBase*>(obj);

			if(resourceHandle->mData && resourceHandle->mData->mUUID != "")
			{
				// NOTE: This will cause Resources::load to be called recursively with resources that contain other
				// resources. This might cause problems. Keep this note here as a warning until I prove otherwise.
				BaseResourceHandle loadedResource = gResources().loadFromUUID(resourceHandle->mData->mUUID);

				if(loadedResource)
					resourceHandle->resolve(loadedResource.getInternalPtr());
			}
		}

		virtual const String& getRTTIName()
		{
			static String name = "ResourceHandleBase";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ResourceHandle;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<ResourceHandleBase>(new ResourceHandleBase());
		}
	};
}