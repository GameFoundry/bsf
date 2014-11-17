#include "BsGpuParamBlockBuffer.h"
#include "BsGpuParamBlock.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine
{
	GpuParamBlockBufferCore::GpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage)
		:mSize(size), mUsage(usage)
	{
	}

	GpuParamBlockBuffer::GpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage)
		:mSize(size), mUsage(usage)
	{
		mParamBlock = bs_shared_ptr<GpuParamBlock>(size);
	}

	SPtr<GpuParamBlockBufferCore> GpuParamBlockBuffer::getCore() const
	{
		return std::static_pointer_cast<GpuParamBlockBufferCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> GpuParamBlockBuffer::createCore() const
	{
		return HardwareBufferCoreManager::instance().createGpuParamBlockBufferInternal(mSize, mUsage);
	}

	GenericGpuParamBlockBufferCore::GenericGpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage)
		:GpuParamBlockBufferCore(size, usage), mData(nullptr)
	{ }

	void GenericGpuParamBlockBufferCore::writeData(const UINT8* data)
	{
		memcpy(mData, data, mSize);
	}

	void GenericGpuParamBlockBufferCore::readData(UINT8* data) const
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

	void GenericGpuParamBlockBufferCore::destroy()
	{
		if(mData != nullptr)
			bs_free<ScratchAlloc>(mData);

		GpuParamBlockBufferCore::destroy();
	}

	static GpuParamBlockBufferPtr create(UINT32 size, GpuParamBlockUsage usage)
	{
		return HardwareBufferManager::instance().createGpuParamBlockBuffer(size, usage);
	}
}