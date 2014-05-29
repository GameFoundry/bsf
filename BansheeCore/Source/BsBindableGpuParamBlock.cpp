#include "BsBindableGpuParamBlock.h"
#include "BsGpuParamBlockBuffer.h"

namespace BansheeEngine
{
	BindableGpuParamBlock::BindableGpuParamBlock()
		:mDirty(true), mData(nullptr), mSize(0)
	{ }

	void BindableGpuParamBlock::uploadToBuffer(GpuParamBlockBufferPtr buffer)
	{
		buffer->writeData(mData);
		mDirty = false;
	}
}