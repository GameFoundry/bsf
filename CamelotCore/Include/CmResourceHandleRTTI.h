#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmResourceHandle.h"
#include "CmResources.h"

namespace CamelotEngine
{
	class CM_EXPORT ResourceHandleRTTI : public RTTIType<ResourceHandleBase, IReflectable, ResourceHandleRTTI>
	{
	private:
		String& getUUID(ResourceHandleBase* obj) { return obj->mData->mUUID; }
		void setUUID(ResourceHandleBase* obj, String& uuid) { obj->mData->mUUID = uuid; } 
	public:
		ResourceHandleRTTI()
		{
			addPlainField("mUUID", 0, &ResourceHandleRTTI::getUUID, &ResourceHandleRTTI::setUUID);
		}

		void onDeserializationEnded(IReflectable* obj)
		{
			ResourceHandleBase* resourceHandle = static_cast<ResourceHandleBase*>(obj);

			if(resourceHandle->mData && resourceHandle->mData->mUUID != "")
			{
				// NOTE: This will cause Resources::load to be called recursively with resources that contain other
				// resources. This might cause problems. Keep this note here as a warning until I prove otherwise.
				HResource loadedResource = gResources().loadFromUUID(resourceHandle->mData->mUUID);

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