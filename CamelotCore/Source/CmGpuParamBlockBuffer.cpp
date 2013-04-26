#include "CmGpuParamBlockBuffer.h"

namespace CamelotFramework
{
	GpuParamBlockBuffer::GpuParamBlockBuffer()
		:mSize(0), mUsage(GPBU_DYNAMIC)
	{

	}

	GpuParamBlockBuffer::~GpuParamBlockBuffer()
	{

	}

	void GpuParamBlockBuffer::initialize(UINT32 size, GpuParamBlockUsage usage)
	{
		mSize = size;
		mUsage = usage;

		CoreObject::initialize();
	}

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
		mData = CM_NEW_BYTES(mSize, ScratchAlloc);
		memset(mData, 0, mSize);

		GpuParamBlockBuffer::initialize_internal();
	}

	void GenericGpuParamBlockBuffer::destroy_internal()
	{
		if(mData != nullptr)
			CM_DELETE_BYTES(mData, ScratchAlloc);

		GpuParamBlockBuffer::destroy_internal();
	}
}