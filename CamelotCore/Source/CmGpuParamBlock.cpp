#include "CmGpuParamBlock.h"
#include "CmGpuParamDesc.h"
#include "CmHardwareBufferManager.h"
#include "CmException.h"

namespace CamelotFramework
{
	GpuParamBlockBuffer::GpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage)
		:mData(CM_NEW_BYTES(size, ScratchAlloc)), mSize(size), mUsage(usage)
	{
		memset(mData, 0, mSize);
	}

	GpuParamBlockBuffer::~GpuParamBlockBuffer()
	{
		if(mData != nullptr)
			CM_DELETE_BYTES(mData, ScratchAlloc);
	}

	void GpuParamBlockBuffer::writeAll(const void* data)
	{
		memcpy(mData, data, mSize);
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

	GpuParamBlock::GpuParamBlock()
		:mDirty(true), mUsage(GPBU_DYNAMIC), mData(nullptr), mBuffer(nullptr)
	{
	}

	GpuParamBlock::~GpuParamBlock()
	{
		if(mData != nullptr)
			CM_DELETE_BYTES(mData, ScratchAlloc);
	}

	void GpuParamBlock::initialize(const GpuParamBlockDesc& desc, GpuParamBlockUsage usage)
	{
		mSize = desc.blockSize * sizeof(UINT32);
		mData = CM_NEW_BYTES(mSize, ScratchAlloc);
		memset(mData, 0, mSize);

		mUsage = usage;

		CoreObject::initialize();
	}

	void GpuParamBlock::initialize_internal()
	{
		mBuffer = createBuffer();

		CoreObject::initialize_internal();
	}

	void GpuParamBlock::destroy_internal()
	{
		if(mBuffer != nullptr)
			CM_DELETE(mBuffer, GpuParamBlockBuffer, PoolAlloc);

		CoreObject::destroy_internal();
	}

	GpuParamBlockBuffer* GpuParamBlock::createBuffer() const
	{
		return CM_NEW(GpuParamBlockBuffer, PoolAlloc) GpuParamBlockBuffer(mSize, mUsage);
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

	void GpuParamBlock::updateBuffer()
	{
		if(mDirty)
		{
			mDirty = false;

			// Need to copy the data, as non-render threads might modify
			// the data before render thread has a chance to process it
			// TODO - Use an allocator
			UINT8* dataCopy = CM_NEW_BYTES(mSize, ScratchAlloc);
			memcpy(dataCopy, mData, mSize);

			queueGpuCommand(getThisPtr(), boost::bind(&GpuParamBlock::updateBuffer_internal, this, dataCopy));
		}
	}

	void GpuParamBlock::updateBuffer_internal(UINT8* data)
	{
		assert(mBuffer != nullptr);

		mBuffer->writeAll(data);

		CM_DELETE_BYTES(data, ScratchAlloc);
	}

	GpuParamBlockPtr GpuParamBlock::create(const GpuParamBlockDesc& desc)
	{
		return HardwareBufferManager::instance().createGpuParamBlock(desc);
	}
}