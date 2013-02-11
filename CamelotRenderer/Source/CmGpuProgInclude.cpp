#include "CmGpuProgInclude.h"

namespace CamelotEngine
{
	GpuProgInclude::GpuProgInclude(const String& includeString)
		:mString(includeString)
	{

	}

	GpuProgIncludeHandle GpuProgInclude::create(const String& includeString)
	{
		GpuProgIncludePtr gpuProgIncludePtr = GpuProgIncludePtr(new GpuProgInclude(includeString));
		gpuProgIncludePtr->setThisPtr(gpuProgIncludePtr);
		gpuProgIncludePtr->initialize();

		return static_resource_cast<GpuProgInclude>(Resource::_createResourceHandle(gpuProgIncludePtr));
	}
}