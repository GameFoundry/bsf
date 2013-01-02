#include "CmGpuParamBlock.h"
#include "CmException.h"

namespace CamelotEngine
{
	GpuParamBlock::GpuParamBlock(UINT32 size)
		:mSize(size), mDirty(true)
	{
		mData = new UINT8[size];
		memset(mData, 0, size);
	}

	GpuParamBlock::~GpuParamBlock()
	{
		delete [] mData;
	}

	void GpuParamBlock::write(UINT32 offset, const void* data, UINT32 size)
	{
		if(offset < 0 || (offset + size) >= mSize)
		{
			CM_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}

		memcpy(mData + offset, data, size);

		mDirty = true;
	}

	void GpuParamBlock::zeroOut(UINT32 offset, UINT32 size)
	{
		if(offset < 0 || (offset + size) >= mSize)
		{
			CM_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}

		memset(mData + offset, 0, size);

		mDirty = true;
	}

	void GpuParamBlock::updateIfDirty()
	{
		// Do nothing
	}
}