#include "BsGpuParamBlockBuffer.h"
#include "BsHardwareBufferManager.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	GpuParamBlockBufferCore::GpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage)
		:mSize(size), mUsage(usage), mCachedData(nullptr), mGPUBufferDirty(false)
	{
		if (mSize > 0)
			mCachedData = (UINT8*)bs_alloc(mSize);

		memset(mCachedData, 0, mSize);
	}

	GpuParamBlockBufferCore::~GpuParamBlockBufferCore()
	{
		if (mCachedData != nullptr)
			bs_free(mCachedData);
	}

	void GpuParamBlockBufferCore::write(UINT32 offset, const void* data, UINT32 size)
	{
#if BS_DEBUG_MODE
		if (offset < 0 || (offset + size) > mSize)
		{
			BS_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memcpy(mCachedData + offset, data, size);
		mGPUBufferDirty = true;
	}

	void GpuParamBlockBufferCore::read(UINT32 offset, void* data, UINT32 size)
	{
#if BS_DEBUG_MODE
		if (offset < 0 || (offset + size) > mSize)
		{
			BS_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memcpy(data, mCachedData + offset, size);
	}

	void GpuParamBlockBufferCore::zeroOut(UINT32 offset, UINT32 size)
	{
#if BS_DEBUG_MODE
		if (offset < 0 || (offset + size) > mSize)
		{
			BS_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memset(mCachedData + offset, 0, size);
		mGPUBufferDirty = true;
	}

	void GpuParamBlockBufferCore::flushToGPU()
	{
		if (mGPUBufferDirty)
		{
			writeToGPU(mCachedData);
			mGPUBufferDirty = false;
		}
	}

	void GpuParamBlockBufferCore::syncToCore(const CoreSyncData& data)
	{
		assert(mSize == data.getBufferSize());

		write(0, data.getBuffer(), data.getBufferSize());
	}

	SPtr<GpuParamBlockBufferCore> GpuParamBlockBufferCore::create(UINT32 size, GpuParamBlockUsage usage)
	{
		return HardwareBufferCoreManager::instance().createGpuParamBlockBuffer(size, usage);
	}

	GpuParamBlockBuffer::GpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage)
		:mSize(size), mUsage(usage), mCachedData(nullptr)
	{
		if (mSize > 0)
			mCachedData = (UINT8*)bs_alloc(mSize);

		memset(mCachedData, 0, mSize);
	}

	GpuParamBlockBuffer::~GpuParamBlockBuffer()
	{
		if (mCachedData != nullptr)
			bs_free(mCachedData);
	}

	void GpuParamBlockBuffer::write(UINT32 offset, const void* data, UINT32 size)
	{
#if BS_DEBUG_MODE
		if (offset < 0 || (offset + size) > mSize)
		{
			BS_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memcpy(mCachedData + offset, data, size);
		markCoreDirty();
	}

	void GpuParamBlockBuffer::read(UINT32 offset, void* data, UINT32 size)
	{
#if BS_DEBUG_MODE
		if (offset < 0 || (offset + size) > mSize)
		{
			BS_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memcpy(data, mCachedData + offset, size);
	}

	void GpuParamBlockBuffer::zeroOut(UINT32 offset, UINT32 size)
	{
#if BS_DEBUG_MODE
		if (offset < 0 || (offset + size) > mSize)
		{
			BS_EXCEPT(InvalidParametersException, "Wanted range is out of buffer bounds. " \
				"Available range: 0 .. " + toString(mSize) + ". " \
				"Wanted range: " + toString(offset) + " .. " + toString(offset + size) + ".");
		}
#endif

		memset(mCachedData + offset, 0, size);
		markCoreDirty();
	}

	SPtr<GpuParamBlockBufferCore> GpuParamBlockBuffer::getCore() const
	{
		return std::static_pointer_cast<GpuParamBlockBufferCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> GpuParamBlockBuffer::createCore() const
	{
		return HardwareBufferCoreManager::instance().createGpuParamBlockBufferInternal(mSize, mUsage);
	}

	CoreSyncData GpuParamBlockBuffer::syncToCore(FrameAlloc* allocator)
	{
		UINT8* buffer = allocator->alloc(mSize);
		read(0, buffer, mSize);

		return CoreSyncData(buffer, mSize);
	}

	GpuParamBlockBufferPtr GpuParamBlockBuffer::create(UINT32 size, GpuParamBlockUsage usage)
	{
		return HardwareBufferManager::instance().createGpuParamBlockBuffer(size, usage);
	}

	GenericGpuParamBlockBufferCore::GenericGpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage)
		:GpuParamBlockBufferCore(size, usage), mData(nullptr)
	{ }

	GenericGpuParamBlockBufferCore::~GenericGpuParamBlockBufferCore()
	{
		if (mData != nullptr)
			bs_free<ScratchAlloc>(mData);
	}

	void GenericGpuParamBlockBufferCore::writeToGPU(const UINT8* data)
	{
		memcpy(mData, data, mSize);
	}

	void GenericGpuParamBlockBufferCore::readFromGPU(UINT8* data) const
	{
		memcpy(data, mData, mSize);
	}

	void GenericGpuParamBlockBufferCore::initialize()
	{
		if (mSize > 0)
			mData = (UINT8*)bs_alloc<ScratchAlloc>(mSize);
		else
			mData = nullptr;

		memset(mData, 0, mSize);

		GpuParamBlockBufferCore::initialize();
	}
}