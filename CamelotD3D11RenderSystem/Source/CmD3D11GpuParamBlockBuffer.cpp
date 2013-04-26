#include "CmD3D11GpuParamBlockBuffer.h"
#include "CmD3D11HardwareBuffer.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"

namespace CamelotFramework
{
	D3D11GpuParamBlockBuffer::D3D11GpuParamBlockBuffer()
		:mBuffer(nullptr)
	{

	}

	void D3D11GpuParamBlockBuffer::initialize_internal()
	{
		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = d3d11rs->getPrimaryDevice();

		if(mUsage == GPBU_STATIC)
			mBuffer = CM_NEW(D3D11HardwareBuffer, PoolAlloc) D3D11HardwareBuffer(D3D11HardwareBuffer::BT_CONSTANT, GBU_STATIC, 1, mSize, device);
		else if(mUsage == GPBU_DYNAMIC)
			mBuffer = CM_NEW(D3D11HardwareBuffer, PoolAlloc) D3D11HardwareBuffer(D3D11HardwareBuffer::BT_CONSTANT, GBU_DYNAMIC, 1, mSize, device);
		else
			CM_EXCEPT(InternalErrorException, "Invalid gpu param block usage.");

		GpuParamBlockBuffer::initialize_internal();
	}

	void D3D11GpuParamBlockBuffer::destroy_internal()
	{
		if(mBuffer != nullptr)
			CM_DELETE(mBuffer, D3D11HardwareBuffer, PoolAlloc);

		GpuParamBlockBuffer::destroy_internal();
	}

	ID3D11Buffer* D3D11GpuParamBlockBuffer::getD3D11Buffer() const
	{
		return mBuffer->getD3DBuffer();
	}

	void D3D11GpuParamBlockBuffer::writeData(const UINT8* data)
	{
		mBuffer->writeData(0, mSize, data, true);
	}

	void D3D11GpuParamBlockBuffer::readData(UINT8* data) const
	{
		mBuffer->readData(0, mSize, data);
	}
}