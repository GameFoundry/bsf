#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmResourceRef.h"
#include "CmResources.h"

namespace CamelotEngine
{
	class CM_EXPORT ResourceRefDataRTTI : public RTTIType<ResourceRefData, IReflectable, ResourceRefDataRTTI>
	{
	private:
		String& getUUID(ResourceRefData* obj) { return obj->mUUID; }
		void setUUID(ResourceRefData* obj, String& uuid) 
		{ 
			obj->mUUID = uuid; 
		} 
	public:
		ResourceRefDataRTTI()
		{
			addPlainField("mUUID", 0, &ResourceRefDataRTTI::getUUID, &ResourceRefDataRTTI::setUUID);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ResourceRefData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ResourceRefData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<ResourceRefData>(new ResourceRefData());
		}
	};

	class CM_EXPORT ResourceRefRTTI : public RTTIType<ResourceRefBase, IReflectable, ResourceRefRTTI>
	{
	private:
		std::shared_ptr<ResourceRefData> getResData(ResourceRefBase* obj) { return obj->mData; }
		void setResData(ResourceRefBase* obj, std::shared_ptr<ResourceRefData> val) { obj->mData = val; }
	public:
		ResourceRefRTTI()
		{
			addReflectablePtrField("mData", 0, &ResourceRefRTTI::getResData, &ResourceRefRTTI::setResData);
		}

		void onDeserializationEnded(IReflectable* obj)
		{
			ResourceRefBase* resourceRef = static_cast<ResourceRefBase*>(obj);

			if(resourceRef->mData && resourceRef->mData->mUUID != "")
			{
				// NOTE: This will cause Resources::load to be called recursively with resources that contain other
				// resources. This might cause problems. Keep this note here as a warning until I prove otherwise.
				BaseResourceRef loadedResource = gResources().loadFromUUID(resourceRef->mData->mUUID);

				if(loadedResource)
					resourceRef->resolve(loadedResource.getInternalPtr());
			}
		}

		virtual const String& getRTTIName()
		{
			static String name = "ResourceRefBase";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ResourceRef;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<ResourceRefBase>(new ResourceRefBase());
		}
	};
}