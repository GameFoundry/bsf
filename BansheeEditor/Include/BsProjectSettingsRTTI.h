#pragma once

#include "BsEditorPrerequisites.h"
#include "BsProjectSettings.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	class ProjectSettingsRTTI : public RTTIType <ProjectSettings, Settings, ProjectSettingsRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mLastOpenScene);

	public:
		ProjectSettingsRTTI()
		{
			BS_ADD_PLAIN_FIELD(mLastOpenScene, 0);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "ProjectSettings";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_ProjectSettings;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ProjectSettings>();
		}
	};
}