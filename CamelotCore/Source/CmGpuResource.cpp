#include "CmGpuResource.h"
#include "CmGpuResourceRTTI.h"

namespace CamelotFramework
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