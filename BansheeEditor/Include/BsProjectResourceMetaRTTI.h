#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsProjectResourceMeta.h"
#include "BsResourceMetaData.h"
#include "BsImportOptions.h"

namespace BansheeEngine
{
	class ProjectResourceMetaRTTI : public RTTIType<ProjectResourceMeta, IReflectable, ProjectResourceMetaRTTI>
	{
	private:
		String& getUUID(ProjectResourceMeta* obj) { return obj->mUUID; }
		void setUUID(ProjectResourceMeta* obj, String& val) { obj->mUUID = val; } 

		UINT32& getTypeId(ProjectResourceMeta* obj) { return obj->mTypeId; }
		void setTypeId(ProjectResourceMeta* obj, UINT32& val) { obj->mTypeId = val; }

		ResourceMetaDataPtr getResourceMeta(ProjectResourceMeta* obj) { return obj->mResourceMeta; }
		void setResourceMeta(ProjectResourceMeta* obj, ResourceMetaDataPtr val) { obj->mResourceMeta = val; }

		ImportOptionsPtr getImportOptions(ProjectResourceMeta* obj) { return obj->mImportOptions; }
		void setImportOptions(ProjectResourceMeta* obj, ImportOptionsPtr val) { obj->mImportOptions = val; }

	public:
		ProjectResourceMetaRTTI()
		{
			addPlainField("mUUID", 0, &ProjectResourceMetaRTTI::getUUID, &ProjectResourceMetaRTTI::setUUID);
			addReflectablePtrField("mImportOptions", 1, &ProjectResourceMetaRTTI::getImportOptions, &ProjectResourceMetaRTTI::setImportOptions);
			addReflectablePtrField("mResourceMeta", 2, &ProjectResourceMetaRTTI::getResourceMeta, &ProjectResourceMetaRTTI::setResourceMeta);
			addPlainField("mTypeId", 3, &ProjectResourceMetaRTTI::getTypeId, &ProjectResourceMetaRTTI::setTypeId);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ProjectResourceMeta";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ProjectResourceMeta;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ProjectResourceMeta::createEmpty();
		}
	};
}