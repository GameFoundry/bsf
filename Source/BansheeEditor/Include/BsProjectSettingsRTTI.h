//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsEditorPrerequisites.h"
#include "BsProjectSettings.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup RTTI-Impl-Editor
	 *  @{
	 */

	class ProjectSettingsRTTI : public RTTIType <ProjectSettings, Settings, ProjectSettingsRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mLastOpenScene);

	public:
		ProjectSettingsRTTI()
		{
			BS_ADD_PLAIN_FIELD(mLastOpenScene, 0);
		}

		const String& getRTTIName() override
		{
			static String name = "ProjectSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ProjectSettings;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ProjectSettings>();
		}
	};

	/** @} */
	/** @endcond */
}