//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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

		bool& getIncludeInBuild(ProjectResourceMeta* obj) { return obj->mIncludeInBuild; }
		void setIncludeInBuild(ProjectResourceMeta* obj, bool& val) { obj->mIncludeInBuild = val; }

	public:
		ProjectResourceMetaRTTI()
		{
			addPlainField("mUUID", 0, &ProjectResourceMetaRTTI::getUUID, &ProjectResourceMetaRTTI::setUUID);
			addReflectablePtrField("mImportOptions", 1, &ProjectResourceMetaRTTI::getImportOptions, &ProjectResourceMetaRTTI::setImportOptions);
			addReflectablePtrField("mResourceMeta", 2, &ProjectResourceMetaRTTI::getResourceMeta, &ProjectResourceMetaRTTI::setResourceMeta);
			addPlainField("mTypeId", 3, &ProjectResourceMetaRTTI::getTypeId, &ProjectResourceMetaRTTI::setTypeId);
			addPlainField("mIncludeInBuild", 4, &ProjectResourceMetaRTTI::getIncludeInBuild, &ProjectResourceMetaRTTI::setIncludeInBuild);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "ProjectResourceMeta";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_ProjectResourceMeta;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return ProjectResourceMeta::createEmpty();
		}
	};
}