#include "BsManagedSerializableObjectData.h"
#include "BsManagedSerializableObjectDataRTTI.h"

namespace BansheeEngine
{
	RTTITypeBase* ManagedSerializableObjectData::getRTTIStatic()
	{
		return ManagedSerializableObjectDataRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableObjectData::getRTTI() const
	{
		return ManagedSerializableObjectData::getRTTIStatic();
	}
}