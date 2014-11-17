#include "BsD3D11GpuBuffer.h"
#include "BsD3D11GpuBufferView.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11HardwareBuffer.h"
#include "BsD3D11Device.h"
#include "BsD3D11Mappings.h"
#include "BsRenderStats.h"
#include "BsException.h"

namespace BansheeEngine
{
	D3D11GpuBufferCore::D3D11GpuBufferCore(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, 
		GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
		: GpuBufferCore(elementCount, elementSize, type, usage, randomGpuWrite, useCounter), mBuffer(nullptr)
	{ }

	D3D11GpuBufferCore::~D3D11GpuBufferCore()
	{ }

	void D3D11GpuBufferCore::initialize()
	{
		D3D11HardwareBuffer::BufferType bufferType;
		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());

		const GpuBufferProperties& props = getProperties();

		switch (props.getType())
		{
		case GBT_STRUCTURED:
			bufferType = D3D11HardwareBuffer::BT_STRUCTURED;
			break;
		case GBT_RAW:
			bufferType = D3D11HardwareBuffer::BT_RAW;
			break;
		case GBT_INDIRECTARGUMENT:
			bufferType = D3D11HardwareBuffer::BT_INDIRECTARGUMENT;
			break;
		case GBT_APPENDCONSUME:
			bufferType = D3D11HardwareBuffer::BT_APPENDCONSUME;
			break;
		default:
			BS_EXCEPT(InvalidParametersException, "Unsupported buffer type " + toString(props.getType()));
		}

		mBuffer = bs_new<D3D11HardwareBuffer, PoolAlloc>(bufferType, props.getUsage(), props.getElementCount(), props.getElementSize(),
			d3d11rs->getPrimaryDevice(), false, false, props.getRandomGpuWrite(), props.getUseCounter());

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuBuffer);

		GpuBufferCore::initialize();
	}

	void D3D11GpuBufferCore::destroy()
	{
		bs_delete<PoolAlloc>(mBuffer);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuBuffer);

		GpuBufferCore::destroy();
	}

	void* D3D11GpuBufferCore::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
#if BS_PROFILING_ENABLED
		if (options == GBL_READ_ONLY || options == GBL_READ_WRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_GpuBuffer);
		}

		if (options == GBL_READ_WRITE || options == GBL_WRITE_ONLY || options == GBL_WRITE_ONLY_DISCARD || options == GBL_WRITE_ONLY_NO_OVERWRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_GpuBuffer);
		}
#endif

		return mBuffer->lock(offset, length, options);
	}

	void D3D11GpuBufferCore::unlock()
	{
		mBuffer->unlock();
	}

	void D3D11GpuBufferCore::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_GpuBuffer);

		mBuffer->readData(offset, length, pDest);
	}

	void D3D11GpuBufferCore::writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags)
	{
		BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_GpuBuffer);

		mBuffer->writeData(offset, length, pSource, writeFlags);
	}

	void D3D11GpuBufferCore::copyData(GpuBufferCore& srcBuffer, UINT32 srcOffset,
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
		D3D11GpuBufferCore* d3d11SrcBuffer = static_cast<D3D11GpuBufferCore*>(&srcBuffer);

		mBuffer->copyData(*d3d11SrcBuffer->mBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
	}

	ID3D11Buffer* D3D11GpuBufferCore::getDX11Buffer() const
	{ 
		return mBuffer->getD3DBuffer(); 
	}

	GpuBufferView* D3D11GpuBufferCore::createView()
	{
		return bs_new<D3D11GpuBufferView, PoolAlloc>();
	}

	void D3D11GpuBufferCore::destroyView(GpuBufferView* view)
	{
		if(view != nullptr)
			bs_delete<PoolAlloc>(view);
	}
}