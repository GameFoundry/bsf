#include "BsGpuParamBlockBufferProxy.h"

namespace BansheeEngine
{
	GpuParamBlockBufferProxy::GpuParamBlockBufferProxy(UINT32 size)
	{
		block = bs_new<GpuParamBlock>(size);
	}

	GpuParamBlockBufferProxy::~GpuParamBlockBufferProxy()
	{
		bs_delete(block);
	}
}