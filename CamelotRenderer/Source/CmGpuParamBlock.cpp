#include "CmGpuParamBlock.h"
#include "CmGpuParamDesc.h"
#include "CmHardwareBufferManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	GpuParamBlock::GpuParamBlock(const GpuParamBlockDesc& desc)
		:mSize(desc.blockSize), mDirty(true)
	{
		mData = new UINT8[desc.blockSize];
		memset(mData, 0, desc.blockSize);
	}

	GpuParamBlock::~GpuParamBlock()
	{
		delete [] mData;
	}

	void GpuParamBlock::write(UINT32 offset, const void* data, UINT32 size)
	{
#if CM_DEBUG_MODE
		if(offset < 0 || (offset + size) >= mSize)
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
		if(offset < 0 || (offset + size) >= mSize)
		{
			CM_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memset(mData + offset, 0, size);

		mDirty = true;
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
		// Do nothing
	}

	GpuParamBlockPtr GpuParamBlock::create(const GpuParamBlockDesc& desc)
	{
		return HardwareBufferManager::instance().createGpuParamBlock(desc);
	}
}