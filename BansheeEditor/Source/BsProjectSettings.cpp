#include "BsProjectSettings.h"
#include "BsProjectSettingsRTTI.h"

namespace BansheeEngine
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