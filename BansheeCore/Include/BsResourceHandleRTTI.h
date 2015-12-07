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

		void onDeserializationEnded(IReflectable* obj) override
		{
			ResourceHandleBase* resourceHandle = static_cast<ResourceHandleBase*>(obj);

			if(resourceHandle->mData && resourceHandle->mData->mUUID != "")
			{
				HResource loadedResource = gResources()._createResourceHandle(resourceHandle->mData->mUUID);

				if (resourceHandle->mData != nullptr)
					resourceHandle->mData->mRefCount--;

				resourceHandle->mData = loadedResource.mData;

				if (resourceHandle->mData != nullptr)
					resourceHandle->mData->mRefCount++;
			}
		}

		virtual const String& getRTTIName() override
		{
			static String name = "ResourceHandleBase";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_ResourceHandle;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			std::shared_ptr<ResourceHandleBase> obj = bs_shared_ptr<ResourceHandleBase>(new (bs_alloc<ResourceHandleBase>()) ResourceHandleBase());
			obj->mData = bs_shared_ptr_new<ResourceHandleData>();
			obj->mData->mRefCount++;

			return obj;
		}
	};
}