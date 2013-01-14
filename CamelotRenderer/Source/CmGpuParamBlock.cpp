#include "CmGpuParamBlock.h"
#include "CmGpuParamDesc.h"
#include "CmHardwareBufferManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	GpuParamBlock::GpuParamBlock(const GpuParamBlockDesc& desc)
		:mSize(desc.blockSize * sizeof(UINT32)), mOwnsSharedData(true)
	{
		mData = new UINT8[mSize];
		memset(mData, 0, mSize);

		sharedData = new GpuParamBlockSharedData();
		sharedData->mDirty = true;
		sharedData->mInitialized = false;
	}

	GpuParamBlock::~GpuParamBlock()
	{
		delete [] mData;

		if(mOwnsSharedData)
			delete sharedData;
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

		sharedData->mDirty = true;
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

		sharedData->mDirty = true;
	}

	const UINT8* GpuParamBlock::getDataPtr(UINT32 offset) const
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

	void GpuParamBlock::updateIfDirty()
	{
		sharedData->mDirty = false;

		// Do nothing
	}

	GpuParamBlockPtr GpuParamBlock::clone() const
	{
		GpuParamBlockPtr clonedParamBlock(new GpuParamBlock(*this));
		clonedParamBlock->mData = new UINT8[mSize];
		clonedParamBlock->mSize = mSize;
		clonedParamBlock->mOwnsSharedData = false;
		memcpy(clonedParamBlock->mData, mData, mSize);

		return clonedParamBlock;
	}

	GpuParamBlockPtr GpuParamBlock::create(const GpuParamBlockDesc& desc)
	{
		return HardwareBufferManager::instance().createGpuParamBlock(desc);
	}
}