#include "CmBindableGpuParamBlock.h"
#include "CmGpuParamBlockBuffer.h"

namespace CamelotFramework
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