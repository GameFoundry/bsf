#include "CmGpuProgInclude.h"
#include "CmResources.h"

namespace CamelotFramework
{
	GpuProgInclude::GpuProgInclude(const String& includeString)
		:Resource(false), mString(includeString)
	{

	}

	HGpuProgInclude GpuProgInclude::create(const String& includeString)
	{
		return static_resource_cast<GpuProgInclude>(gResources().createResourceHandle(_createPtr(includeString)));
	}

	GpuProgIncludePtr GpuProgInclude::_createPtr(const String& includeString)
	{
		GpuProgIncludePtr gpuProgIncludePtr = cm_core_ptr<GpuProgInclude, PoolAlloc>(
			new (cm_alloc<GpuProgInclude, PoolAlloc>()) GpuProgInclude(includeString));
		gpuProgIncludePtr->setThisPtr(gpuProgIncludePtr);
		gpuProgIncludePtr->initialize();

		return gpuProgIncludePtr;
	}
}