#include "BsGameSettings.h"
#include "BsGameSettingsRTTI.h"

namespace BansheeEngine
{
	RTTITypeBase* GameSettings::getRTTIStatic()
	{
		return GameSettingsRTTI::instance();
	}

	RTTITypeBase* GameSettings::getRTTI() const
	{
		return GameSettings::getRTTIStatic();
	}

}