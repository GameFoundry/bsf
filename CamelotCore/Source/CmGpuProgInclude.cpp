#include "CmGpuProgInclude.h"

namespace CamelotEngine
{
	GpuProgInclude::GpuProgInclude(const String& includeString)
		:Resource(false), mString(includeString)
	{

	}

	GpuProgIncludeHandle GpuProgInclude::create(const String& includeString)
	{
		GpuProgIncludePtr gpuProgIncludePtr = GpuProgIncludePtr(new GpuProgInclude(includeString), &CoreObject::_deleteDelayed);
		gpuProgIncludePtr->setThisPtr(gpuProgIncludePtr);
		gpuProgIncludePtr->initialize();

		return static_resource_cast<GpuProgInclude>(Resource::_createResourceHandle(gpuProgIncludePtr));
	}
}