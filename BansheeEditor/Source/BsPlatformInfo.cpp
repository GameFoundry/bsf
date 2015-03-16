#include "BsPlatformInfo.h"
#include "BsPlatformInfoRTTI.h"

namespace BansheeEngine
{
	PlatformInfo::PlatformInfo()
		:type(PlatformType::Windows)
	{ }

	PlatformInfo::~PlatformInfo()
	{ }

	RTTITypeBase* PlatformInfo::getRTTIStatic()
	{
		return PlatformInfoRTTI::instance();
	}

	RTTITypeBase* PlatformInfo::getRTTI() const
	{
		return PlatformInfo::getRTTIStatic();
	}

	WinPlatformInfo::WinPlatformInfo()
		:is32bit(true)
	{ }

	RTTITypeBase* WinPlatformInfo::getRTTIStatic()
	{
		return WinPlatformInfoRTTI::instance();
	}

	RTTITypeBase* WinPlatformInfo::getRTTI() const
	{
		return WinPlatformInfo::getRTTIStatic();
	}
}