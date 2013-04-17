#include "CmD3D11GpuParamBlock.h"
#include "CmD3D11HardwareBuffer.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"

namespace CamelotFramework
{
	D3D11GpuParamBlockBuffer::D3D11GpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage)
		:GpuParamBlockBuffer(size, usage), mBuffer(nullptr)
	{
		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = d3d11rs->getPrimaryDevice();

		if(mUsage == GPBU_STATIC)
			mBuffer = CM_NEW(D3D11HardwareBuffer, PoolAlloc) D3D11HardwareBuffer(D3D11HardwareBuffer::BT_CONSTANT, GBU_STATIC, 1, mSize, device);
		else if(mUsage == GPBU_DYNAMIC)
			mBuffer = CM_NEW(D3D11HardwareBuffer, PoolAlloc) D3D11HardwareBuffer(D3D11HardwareBuffer::BT_CONSTANT, GBU_DYNAMIC, 1, mSize, device);
		else
			CM_EXCEPT(InternalErrorException, "Invalid gpu param block usage.");
	}

	D3D11GpuParamBlockBuffer::~D3D11GpuParamBlockBuffer()
	{
		if(mBuffer != nullptr)
			CM_DELETE(mBuffer, D3D11HardwareBuffer, PoolAlloc);
	}

	ID3D11Buffer* D3D11GpuParamBlockBuffer::getD3D11Buffer() const
	{
		return mBuffer->getD3DBuffer();
	}

	void D3D11GpuParamBlockBuffer::writeAll(const void* data)
	{
		mBuffer->writeData(0, mSize, data, true);

		GpuParamBlockBuffer::writeAll(data);
	}

	GpuParamBlockBuffer* D3D11GpuParamBlock::createBuffer() const
	{
		return CM_NEW(D3D11GpuParamBlockBuffer, PoolAlloc) D3D11GpuParamBlockBuffer(mSize, mUsage);
	}
}