//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D11GpuParamBlockBuffer.h"
#include "BsD3D11HardwareBuffer.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11Device.h"
#include "BsRenderStats.h"

namespace BansheeEngine
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
			mBuffer = bs_new<D3D11HardwareBuffer, PoolAlloc>(D3D11HardwareBuffer::BT_CONSTANT, GBU_STATIC, 1, mSize, std::ref(device));
		else if(mUsage == GPBU_DYNAMIC)
			mBuffer = bs_new<D3D11HardwareBuffer, PoolAlloc>(D3D11HardwareBuffer::BT_CONSTANT, GBU_DYNAMIC, 1, mSize, std::ref(device));
		else
			BS_EXCEPT(InternalErrorException, "Invalid gpu param block usage.");

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuParamBuffer);

		GpuParamBlockBuffer::initialize_internal();
	}

	void D3D11GpuParamBlockBuffer::destroy_internal()
	{
		if(mBuffer != nullptr)
			bs_delete<PoolAlloc>(mBuffer);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuParamBuffer);

		GpuParamBlockBuffer::destroy_internal();
	}

	ID3D11Buffer* D3D11GpuParamBlockBuffer::getD3D11Buffer() const
	{
		return mBuffer->getD3DBuffer();
	}

	void D3D11GpuParamBlockBuffer::writeData(const UINT8* data)
	{
		mBuffer->writeData(0, mSize, data, BufferWriteType::Discard);

		BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_GpuParamBuffer);
	}

	void D3D11GpuParamBlockBuffer::readData(UINT8* data) const
	{
		mBuffer->readData(0, mSize, data);

		BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_GpuParamBuffer);
	}
}