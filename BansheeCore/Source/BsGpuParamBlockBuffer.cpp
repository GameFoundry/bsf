//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGpuParamBlockBuffer.h"
#include "BsGpuParamBlock.h"

namespace BansheeEngine
{
	GpuParamBlockBuffer::GpuParamBlockBuffer()
		:mSize(0), mUsage(GPBU_DYNAMIC), mParamBlock(nullptr), mCoreParamBlock(nullptr)
	{

	}

	GpuParamBlockBuffer::~GpuParamBlockBuffer()
	{

	}

	void GpuParamBlockBuffer::initialize(UINT32 size, GpuParamBlockUsage usage)
	{
		mSize = size;
		mUsage = usage;

		mParamBlock = bs_shared_ptr<GpuParamBlock>(size);

		CoreObject::initialize();
	}

	GenericGpuParamBlockBuffer::GenericGpuParamBlockBuffer()
		:mData(nullptr)
	{ }

	void GenericGpuParamBlockBuffer::writeData(const UINT8* data)
	{
		memcpy(mData, data, mSize);
	}

	void GenericGpuParamBlockBuffer::readData(UINT8* data) const
	{
		memcpy(data, mData, mSize);
	}

	void GenericGpuParamBlockBuffer::initialize_internal()
	{
		if (mSize > 0)
			mData = (UINT8*)bs_alloc<ScratchAlloc>(mSize);
		else
			mData = nullptr;

		memset(mData, 0, mSize);

		GpuParamBlockBuffer::initialize_internal();
	}

	void GenericGpuParamBlockBuffer::destroy_internal()
	{
		if(mData != nullptr)
			bs_free<ScratchAlloc>(mData);

		GpuParamBlockBuffer::destroy_internal();
	}
}