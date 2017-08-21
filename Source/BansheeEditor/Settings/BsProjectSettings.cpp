//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Settings/BsProjectSettings.h"
#include "RTTI/BsProjectSettingsRTTI.h"

namespace bs
{
	ProjectSettings::ProjectSettings()
	{ }

	RTTITypeBase* ProjectSettings::getRTTIStatic()
	{
		return ProjectSettingsRTTI::instance();
	}

	RTTITypeBase* ProjectSettings::getRTTI() const
	{
		return ProjectSettings::getRTTIStatic();
	}
}