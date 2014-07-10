//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
				// NOTE: This will cause Resources::load to be called recursively with resources that contain other
				// resources. This might cause problems. Keep this note here as a warning until I prove otherwise.
				HResource loadedResource = gResources().loadFromUUID(resourceHandle->mData->mUUID);

				if(loadedResource)
					resourceHandle->_setHandleData(loadedResource.getInternalPtr(), resourceHandle->mData->mUUID);
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