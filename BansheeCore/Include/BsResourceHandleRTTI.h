#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsResourceHandle.h"
#include "BsResources.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT ResourceHandleRTTI : public RTTIType<ResourceHandleBase, IReflectable, ResourceHandleRTTI>
	{
	private:
		String& getUUID(ResourceHandleBase* obj) 
		{ 
			static String Blank = "";

			return obj->mData != nullptr ? obj->mData->mUUID : Blank; 
		}

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
				// Note: Resource system needs to be aware of this handle before this point is reached so it
				// can queue the load. This is handled automatically by Resources but is something to be aware of
				// if you are loading resources in some other way.
				HResource loadedResource = gResources()._getResourceHandle(resourceHandle->mData->mUUID);

				if(loadedResource)
					resourceHandle->_setHandleData(loadedResource.getHandleData());
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
			std::shared_ptr<ResourceHandleBase> obj = bs_shared_ptr<ResourceHandleBase, PoolAlloc>(new (bs_alloc<ResourceHandleBase, PoolAlloc>()) ResourceHandleBase());
			obj->mData = bs_shared_ptr<ResourceHandleData>();

			return obj;
		}
	};
}