#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuParamBlock.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	struct BS_CORE_EXPORT GpuParamBlockBufferProxy
	{
		GpuParamBlockBufferProxy(UINT32 size);
		~GpuParamBlockBufferProxy();

		GpuParamBlock* block;
	};
}