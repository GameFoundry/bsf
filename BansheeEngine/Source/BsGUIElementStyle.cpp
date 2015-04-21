#include "BsGUIElementStyle.h"
#include "BsGUIElementStyleRTTI.h"

namespace BansheeEngine
{
	RTTITypeBase* GUIElementStyle::getRTTIStatic()
	{
		return GUIElementStyleRTTI::instance();
	}

	RTTITypeBase* GUIElementStyle::getRTTI() const
	{
		return GUIElementStyle::getRTTIStatic();
	}
}