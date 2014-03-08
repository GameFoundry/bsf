#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "BsResourceMeta.h"
#include "CmImportOptions.h"

namespace BansheeEditor
{
	class ResourceMetaRTTI : public CM::RTTIType<ResourceMeta, CM::IReflectable, ResourceMetaRTTI>
	{
	private:
		CM::String& getUUID(ResourceMeta* obj) { return obj->mUUID; }
		void setUUID(ResourceMeta* obj, CM::String& val) { obj->mUUID = val; } 

		CM::ImportOptionsPtr getImportOptions(ResourceMeta* obj) { return obj->mImportOptions; }
		void setImportOptions(ResourceMeta* obj, CM::ImportOptionsPtr val) { obj->mImportOptions = val; }

	public:
		ResourceMetaRTTI()
		{
			addPlainField("mUUID", 0, &ResourceMetaRTTI::getUUID, &ResourceMetaRTTI::setUUID);
			addReflectablePtrField("mImportOptions", 1, &ResourceMetaRTTI::getImportOptions, &ResourceMetaRTTI::setImportOptions);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ResourceMeta";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ResourceMeta;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return ResourceMeta::createEmpty();
		}
	};
}