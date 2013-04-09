#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmResources.h"

namespace CamelotEngine
{
	class ResourceMetaDataRTTI : public RTTIType<Resources::ResourceMetaData, IReflectable, ResourceMetaDataRTTI>
	{
		String& getUUID(Resources::ResourceMetaData* obj) { return obj->mUUID; }
		void setUUID(Resources::ResourceMetaData* obj, String& val) { obj->mUUID = val; }

		String& getPath(Resources::ResourceMetaData* obj) { return obj->mPath; }
		void setPath(Resources::ResourceMetaData* obj, String& val) { obj->mPath = val; }

	public:
		ResourceMetaDataRTTI()
		{
			addPlainField("mUUID", 0, &ResourceMetaDataRTTI::getUUID, &ResourceMetaDataRTTI::setUUID);
			addPlainField("mPath", 1, &ResourceMetaDataRTTI::getPath, &ResourceMetaDataRTTI::setPath);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<Resources::ResourceMetaData>(CM_NEW(Resources::ResourceMetaData, PoolAlloc) Resources::ResourceMetaData(),
				&MemAllocDeleter<Resources::ResourceMetaData, PoolAlloc>::deleter);
		}

		virtual const String& getRTTIName() 
		{
			static String name = "ResourceMetaData";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_ResourceMetaData;
		}
	};
}