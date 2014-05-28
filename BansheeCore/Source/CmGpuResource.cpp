#include "CmGpuResource.h"
#include "CmGpuResourceRTTI.h"

namespace BansheeEngine
{
	RTTITypeBase* GpuResource::getRTTIStatic()
	{
		return GpuResourceRTTI::instance();
	}

	RTTITypeBase* GpuResource::getRTTI() const
	{
		return GpuResource::getRTTIStatic();
	}
}