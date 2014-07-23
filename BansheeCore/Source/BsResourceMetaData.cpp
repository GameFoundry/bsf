#include "BsResourceMetaData.h"
#include "BsResourceMetaDataRTTI.h"

namespace BansheeEngine
{
	RTTITypeBase* ResourceMetaData::getRTTIStatic()
	{
		return ResourceMetaDataRTTI::instance();
	}

	RTTITypeBase* ResourceMetaData::getRTTI() const
	{
		return ResourceMetaData::getRTTIStatic();
	}
}