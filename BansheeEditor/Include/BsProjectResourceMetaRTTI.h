#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsProjectResourceMeta.h"
#include "BsResourceMetaData.h"
#include "BsImportOptions.h"

namespace BansheeEngine
{
	class ResourceMetaRTTI : public RTTIType<ProjectResourceMeta, IReflectable, ResourceMetaRTTI>
	{
	private:
		String& getUUID(ProjectResourceMeta* obj) { return obj->mUUID; }
		void setUUID(ProjectResourceMeta* obj, String& val) { obj->mUUID = val; } 

		ResourceMetaDataPtr getResourceMeta(ProjectResourceMeta* obj) { return obj->mResourceMeta; }
		void setResourceMeta(ProjectResourceMeta* obj, ResourceMetaDataPtr val) { obj->mResourceMeta = val; }

		ImportOptionsPtr getImportOptions(ProjectResourceMeta* obj) { return obj->mImportOptions; }
		void setImportOptions(ProjectResourceMeta* obj, ImportOptionsPtr val) { obj->mImportOptions = val; }

	public:
		ResourceMetaRTTI()
		{
			addPlainField("mUUID", 0, &ResourceMetaRTTI::getUUID, &ResourceMetaRTTI::setUUID);
			addReflectablePtrField("mImportOptions", 1, &ResourceMetaRTTI::getImportOptions, &ResourceMetaRTTI::setImportOptions);
			addReflectablePtrField("mResourceMeta", 2, &ResourceMetaRTTI::getResourceMeta, &ResourceMetaRTTI::setResourceMeta);
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
			return ProjectResourceMeta::createEmpty();
		}
	};
}