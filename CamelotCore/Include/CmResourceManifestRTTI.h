#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmResourceManifest.h"

namespace CamelotFramework
{
	struct ResourceManifestEntry
	{
		String uuid;
		WString path;
	};

	template<> struct RTTIPlainType<ResourceManifestEntry>
	{	
		enum { id = TID_ResourceManifestEntry }; enum { hasDynamicSize = 1 };

		static void toMemory(const ResourceManifestEntry& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			memcpy(memory, &size, sizeof(UINT32));
			memory += sizeof(UINT32);

			rttiWriteElem(data.uuid, memory);
			rttiWriteElem(data.path, memory);
		}

		static UINT32 fromMemory(ResourceManifestEntry& data, char* memory)
		{ 
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);

			rttiReadElem(data.uuid, memory);
			rttiReadElem(data.path, memory);

			return size;
		}

		static UINT32 getDynamicSize(const ResourceManifestEntry& data)	
		{ 
			UINT64 dataSize = sizeof(UINT32);
			dataSize += rttiGetElemSize(data.uuid);
			dataSize += rttiGetElemSize(data.path);

			return (UINT32)dataSize;
		}	
	}; 

	class CM_EXPORT ResourceManifestRTTI : public RTTIType<ResourceManifest, IReflectable, ResourceManifestRTTI>
	{
	private:
		Map<String, WPath>::type& getUUIDMap(ResourceManifest* obj) 
		{ 
			return obj->mUUIDToFilePath;
		}

		void setUUIDMap(ResourceManifest* obj, Map<String, WPath>::type& val) 
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
			addPlainField("mUUIDToFilePath", 0, &ResourceManifestRTTI::getUUIDMap, &ResourceManifestRTTI::setUUIDMap);
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
			std::shared_ptr<ResourceManifest> obj = cm_shared_ptr<ResourceManifest>();

			return obj;
		}
	};
}