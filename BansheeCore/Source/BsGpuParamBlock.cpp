#include "BsGpuParamBlock.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsHardwareBufferManager.h"
#include "BsFrameAlloc.h"
#include "BsException.h"

namespace BansheeEngine
{
	GpuParamBlock::GpuParamBlock(UINT32 size)
		:mDirty(true), mData(nullptr), mSize(size), mAlloc(nullptr)
	{
		if (mSize > 0)
			mData = (UINT8*)bs_alloc(mSize);

		memset(mData, 0, mSize);
	}

	GpuParamBlock::GpuParamBlock(FrameAlloc* alloc, UINT32 size)
		:mDirty(true), mData(nullptr), mSize(size), mAlloc(alloc)
	{
		if (mSize > 0)
			mData = alloc->alloc(mSize);

		memset(mData, 0, mSize);
	}

	GpuParamBlock::~GpuParamBlock()
	{
		if (mData != nullptr)
		{
			if (mAlloc != nullptr)
				mAlloc->dealloc(mData);
			else
				bs_free(mData);
		}
	}

	void GpuParamBlock::write(UINT32 offset, const void* data, UINT32 size)
	{
#if BS_DEBUG_MODE
		if(offset < 0 || (offset + size) > mSize)
		{
			BS_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memcpy(mData + offset, data, size);

		mDirty = true;
	}

	void GpuParamBlock::read(UINT32 offset, void* data, UINT32 size)
	{
#if BS_DEBUG_MODE
		if(offset < 0 || (offset + size) > mSize)
		{
			BS_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memcpy(data, mData + offset, size);
	}

	void GpuParamBlock::zeroOut(UINT32 offset, UINT32 size)
	{
#if BS_DEBUG_MODE
		if(offset < 0 || (offset + size) > mSize)
		{
			BS_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memset(mData + offset, 0, size);

		mDirty = true;
	}

	void GpuParamBlock::uploadToBuffer(const GpuParamBlockBufferPtr& buffer)
	{
		buffer->getCore()->writeData(mData);
		mDirty = false;
	}
}