#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsResourceMeta.h"
#include "BsImportOptions.h"

namespace BansheeEngine
{
	class ResourceMetaRTTI : public RTTIType<ResourceMeta, IReflectable, ResourceMetaRTTI>
	{
	private:
		String& getUUID(ResourceMeta* obj) { return obj->mUUID; }
		void setUUID(ResourceMeta* obj, String& val) { obj->mUUID = val; } 

		ImportOptionsPtr getImportOptions(ResourceMeta* obj) { return obj->mImportOptions; }
		void setImportOptions(ResourceMeta* obj, ImportOptionsPtr val) { obj->mImportOptions = val; }

	public:
		ResourceMetaRTTI()
		{
			addPlainField("mUUID", 0, &ResourceMetaRTTI::getUUID, &ResourceMetaRTTI::setUUID);
			addReflectablePtrField("mImportOptions", 1, &ResourceMetaRTTI::getImportOptions, &ResourceMetaRTTI::setImportOptions);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ResourceMeta";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ResourceMeta;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ResourceMeta::createEmpty();
		}
	};
}