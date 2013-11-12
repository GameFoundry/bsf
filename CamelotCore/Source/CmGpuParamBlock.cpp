#include "CmGpuParamBlock.h"
#include "CmGpuParamDesc.h"
#include "CmGpuParamBlockBuffer.h"
#include "CmHardwareBufferManager.h"
#include "CmException.h"

namespace CamelotFramework
{
	GpuParamBlock::GpuParamBlock(UINT32 size)
		:mDirty(true), mData(nullptr), mSize(size)
	{
		mData = (UINT8*)cm_alloc<ScratchAlloc>(mSize);
		memset(mData, 0, mSize);
	}

	GpuParamBlock::GpuParamBlock(GpuParamBlock* otherBlock)
	{
		mSize = otherBlock->mSize;
		mData = (UINT8*)cm_alloc<ScratchAlloc>(mSize);
		write(0, otherBlock->getData(), otherBlock->getSize());
		mDirty = otherBlock->mDirty;
	}

	GpuParamBlock::~GpuParamBlock()
	{
		if(mData != nullptr)
			cm_free<ScratchAlloc>(mData);
	}

	void GpuParamBlock::write(UINT32 offset, const void* data, UINT32 size)
	{
#if CM_DEBUG_MODE
		if(offset < 0 || (offset + size) > mSize)
		{
			CM_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memcpy(mData + offset, data, size);

		mDirty = true;
	}

	void GpuParamBlock::zeroOut(UINT32 offset, UINT32 size)
	{
#if CM_DEBUG_MODE
		if(offset < 0 || (offset + size) > mSize)
		{
			CM_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memset(mData + offset, 0, size);

		mDirty = true;
	}
}