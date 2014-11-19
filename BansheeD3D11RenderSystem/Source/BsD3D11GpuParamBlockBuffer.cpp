#include "BsD3D11GpuParamBlockBuffer.h"
#include "BsD3D11HardwareBuffer.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11Device.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	D3D11GpuParamBlockBufferCore::D3D11GpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage)
		:GpuParamBlockBufferCore(size, usage), mBuffer(nullptr)
	{

	}

	void D3D11GpuParamBlockBufferCore::initialize()
	{
		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = d3d11rs->getPrimaryDevice();

		if(mUsage == GPBU_STATIC)
			mBuffer = bs_new<D3D11HardwareBuffer, PoolAlloc>(D3D11HardwareBuffer::BT_CONSTANT, GBU_STATIC, 1, mSize, std::ref(device));
		else if(mUsage == GPBU_DYNAMIC)
			mBuffer = bs_new<D3D11HardwareBuffer, PoolAlloc>(D3D11HardwareBuffer::BT_CONSTANT, GBU_DYNAMIC, 1, mSize, std::ref(device));
		else
			BS_EXCEPT(InternalErrorException, "Invalid gpu param block usage.");

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuParamBuffer);

		GpuParamBlockBufferCore::initialize();
	}

	void D3D11GpuParamBlockBufferCore::destroy()
	{
		if(mBuffer != nullptr)
			bs_delete<PoolAlloc>(mBuffer);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuParamBuffer);

		GpuParamBlockBufferCore::destroy();
	}

	ID3D11Buffer* D3D11GpuParamBlockBufferCore::getD3D11Buffer() const
	{
		return mBuffer->getD3DBuffer();
	}

	void D3D11GpuParamBlockBufferCore::writeToGPU(const UINT8* data)
	{
		mBuffer->writeData(0, mSize, data, BufferWriteType::Discard);

		BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_GpuParamBuffer);
	}

	void D3D11GpuParamBlockBufferCore::readFromGPU(UINT8* data) const
	{
		mBuffer->readData(0, mSize, data);

		BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_GpuParamBuffer);
	}
}