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
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Editor
	 *  @{
	 */

	class ProjectResourceMetaRTTI : public RTTIType<ProjectResourceMeta, IReflectable, ProjectResourceMetaRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mName)
		BS_PLAIN_MEMBER(mUUID)
		BS_PLAIN_MEMBER(mTypeId)
		BS_REFLPTR_MEMBER(mResourceMeta)
			
	public:
		ProjectResourceMetaRTTI()
		{
			BS_ADD_PLAIN_FIELD(mName, 0);
			BS_ADD_PLAIN_FIELD(mUUID, 1);
			BS_ADD_PLAIN_FIELD(mTypeId, 2);
			BS_ADD_REFLPTR_FIELD(mResourceMeta, 3);
		}

		const String& getRTTIName() override
		{
			static String name = "ProjectResourceMeta";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ProjectResourceMeta;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return ProjectResourceMeta::createEmpty();
		}
	};

	class ProjectFileMetaRTTI : public RTTIType<ProjectFileMeta, IReflectable, ProjectFileMetaRTTI>
	{
	private:
		BS_REFLPTR_MEMBER(mImportOptions)
		BS_PLAIN_MEMBER(mIncludeInBuild)
		BS_REFLPTR_MEMBER_VEC(mResourceMetaData)

	public:
		ProjectFileMetaRTTI()
		{
			BS_ADD_REFLPTR_FIELD(mImportOptions, 1);
			BS_ADD_PLAIN_FIELD(mIncludeInBuild, 4);
			BS_ADD_REFLPTR_FIELD_ARR(mResourceMetaData, 5);
		}

		const String& getRTTIName() override
		{
			static String name = "ProjectFileMeta";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ProjectFileMeta;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return ProjectFileMeta::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}