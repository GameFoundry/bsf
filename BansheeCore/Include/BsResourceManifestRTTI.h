#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsResourceManifest.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT ResourceManifestRTTI : public RTTIType<ResourceManifest, IReflectable, ResourceManifestRTTI>
	{
	private:
		String& getName(ResourceManifest* obj)
		{
			return obj->mName;
		}

		void setName(ResourceManifest* obj, String& val)
		{
			obj->mName = val;
		}

		UnorderedMap<String, Path>& getUUIDMap(ResourceManifest* obj)
		{ 
			return obj->mUUIDToFilePath;
		}

		void setUUIDMap(ResourceManifest* obj, UnorderedMap<String, Path>& val)
		{ 
			obj->mUUIDToFilePath = val; 

			obj->mFilePathToUUID.clear();

			for(auto& entry : obj->mUUIDToFilePath)
			{
				obj->mFilePathToUUID[entry.second] = entry.first;
			}
		} 
	public:
		ResourceManifestRTTI()
		{
			addPlainField("mName", 0, &ResourceManifestRTTI::getName, &ResourceManifestRTTI::setName);
			addPlainField("mUUIDToFilePath", 1, &ResourceManifestRTTI::getUUIDMap, &ResourceManifestRTTI::setUUIDMap);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ResourceManifest";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ResourceManifest;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ResourceManifest::createEmpty();
		}
	};
}