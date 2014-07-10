//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGpuParamBlock.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsHardwareBufferManager.h"
#include "BsException.h"

namespace BansheeEngine
{
	GpuParamBlock::GpuParamBlock(UINT32 size)
		:mDirty(true), mData(nullptr), mSize(size)
	{
		if (mSize > 0)
			mData = (UINT8*)bs_alloc<ScratchAlloc>(mSize);

		memset(mData, 0, mSize);
	}

	GpuParamBlock::GpuParamBlock(GpuParamBlock* otherBlock)
	{
		mSize = otherBlock->mSize;

		if (mSize > 0)
			mData = (UINT8*)bs_alloc<ScratchAlloc>(mSize);
		else
			mData = nullptr;

		write(0, otherBlock->getData(), otherBlock->getSize());
		mDirty = otherBlock->mDirty;
	}

	GpuParamBlock::~GpuParamBlock()
	{
		if(mData != nullptr)
			bs_free<ScratchAlloc>(mData);
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
		buffer->writeData(mData);
		mDirty = false;
	}
}