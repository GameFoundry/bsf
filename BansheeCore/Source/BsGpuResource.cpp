#include "BsGpuResource.h"
#include "BsGpuResourceRTTI.h"

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