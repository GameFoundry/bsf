#include "CmGpuParamBlock.h"
#include "CmGpuParamDesc.h"
#include "CmHardwareBufferManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	GpuParamBlockBuffer::GpuParamBlockBuffer(const GpuParamBlockDesc& desc)
		:mSize(desc.blockSize * sizeof(UINT32)), mOwnsSharedData(true)
	{
		mData = new UINT8[mSize];
		memset(mData, 0, mSize);

		sharedData = new GpuParamBlockSharedData();
		sharedData->mDirty = true;
		sharedData->mInitialized = false;
	}

	GpuParamBlockBuffer::~GpuParamBlockBuffer()
	{
		delete [] mData;

		if(mOwnsSharedData)
			delete sharedData;
	}

	void GpuParamBlockBuffer::write(UINT32 offset, const void* data, UINT32 size)
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

		sharedData->mDirty = true;
	}

	void GpuParamBlockBuffer::zeroOut(UINT32 offset, UINT32 size)
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

		sharedData->mDirty = true;
	}

	const UINT8* GpuParamBlockBuffer::getDataPtr(UINT32 offset) const
	{
#if CM_DEBUG_MODE
		if(offset < 0 || offset >= mSize)
		{
			CM_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset) + ".");
		}
#endif

		return &mData[offset];
	}

	void GpuParamBlockBuffer::updateIfDirty()
	{
		sharedData->mDirty = false;

		// Do nothing
	}

	GpuParamBlockBufferPtr GpuParamBlockBuffer::clone() const
	{
		GpuParamBlockBufferPtr clonedParamBlock(new GpuParamBlockBuffer(*this));
		clonedParamBlock->mData = new UINT8[mSize];
		clonedParamBlock->mSize = mSize;
		clonedParamBlock->mOwnsSharedData = false;
		memcpy(clonedParamBlock->mData, mData, mSize);

		return clonedParamBlock;
	}

	GpuParamBlockBufferPtr GpuParamBlockBuffer::create(const GpuParamBlockDesc& desc)
	{
		return HardwareBufferManager::instance().createGpuParamBlockBuffer(desc);
	}
}