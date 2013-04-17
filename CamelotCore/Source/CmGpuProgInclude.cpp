#include "CmGpuProgInclude.h"

namespace CamelotFramework
{
	GpuProgInclude::GpuProgInclude(const String& includeString)
		:Resource(false), mString(includeString)
	{

	}

	HGpuProgInclude GpuProgInclude::create(const String& includeString)
	{
		GpuProgIncludePtr gpuProgIncludePtr = GpuProgIncludePtr(CM_NEW(GpuProgInclude, PoolAlloc) GpuProgInclude(includeString), 
			&CoreObject::_deleteDelayed<GpuProgInclude, PoolAlloc>);
		gpuProgIncludePtr->setThisPtr(gpuProgIncludePtr);
		gpuProgIncludePtr->initialize();

		return static_resource_cast<GpuProgInclude>(Resource::_createResourceHandle(gpuProgIncludePtr));
	}
}