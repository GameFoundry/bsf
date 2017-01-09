//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD3D11GpuParamBlockBuffer.h"
#include "BsD3D11HardwareBuffer.h"
#include "BsD3D11RenderAPI.h"
#include "BsD3D11Device.h"
#include "BsRenderStats.h"

namespace bs { namespace ct
{
	D3D11GpuParamBlockBuffer::D3D11GpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage, 
		GpuDeviceFlags deviceMask)
		:GpuParamBlockBuffer(size, usage, deviceMask), mBuffer(nullptr)
	{
		assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
	}

	D3D11GpuParamBlockBuffer::~D3D11GpuParamBlockBuffer()
	{
		if (mBuffer != nullptr)
			bs_delete(mBuffer);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuParamBuffer);
	}

	void D3D11GpuParamBlockBuffer::initialize()
	{
		D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
		D3D11Device& device = d3d11rs->getPrimaryDevice();

		if(mUsage == GPBU_STATIC)
			mBuffer = bs_new<D3D11HardwareBuffer>(D3D11HardwareBuffer::BT_CONSTANT, GBU_STATIC, 1, mSize, std::ref(device));
		else if(mUsage == GPBU_DYNAMIC)
			mBuffer = bs_new<D3D11HardwareBuffer>(D3D11HardwareBuffer::BT_CONSTANT, GBU_DYNAMIC, 1, mSize, std::ref(device));
		else
			BS_EXCEPT(InternalErrorException, "Invalid gpu param block usage.");

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuParamBuffer);

		GpuParamBlockBuffer::initialize();
	}


	ID3D11Buffer* D3D11GpuParamBlockBuffer::getD3D11Buffer() const
	{
		return mBuffer->getD3DBuffer();
	}

	void D3D11GpuParamBlockBuffer::writeToGPU(const UINT8* data, UINT32 queueIdx)
	{
		mBuffer->writeData(0, mSize, data, BWT_DISCARD);

		BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_GpuParamBuffer);
	}
}}