#include "BsManagedResourceMetaData.h"
#include "BsManagedResourceMetaDataRTTI.h"

namespace BansheeEngine
{
	RTTITypeBase* ManagedResourceMetaData::getRTTIStatic()
	{
		return ManagedResourceMetaDataRTTI::instance();
	}

	RTTITypeBase* ManagedResourceMetaData::getRTTI() const
	{
		return ManagedResourceMetaData::getRTTIStatic();
	}
}