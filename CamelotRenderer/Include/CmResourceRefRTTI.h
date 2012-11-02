#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmResourceRef.h"

namespace CamelotEngine
{
	class CM_EXPORT ResourceRefDataRTTI : public RTTIType<ResourceRefData, IReflectable, ResourceRefDataRTTI>
	{
	private:
		String& getUUID(ResourceRefData* obj) { return obj->mUUID; }
		void setUUID(ResourceRefData* obj, String& uuid) 
		{ 
			obj->mUUID = uuid; 

			if(uuid != "")
			{
				// TODO - I probably want to load the resource here
				//   - Important: consider that user might just want to load the level meta data and not the actual resources
				//     (Maybe he wants to stream them in as player goes through the world?)
				//       - Although this should probably be handled on a higher level. Here we just load.
			}
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