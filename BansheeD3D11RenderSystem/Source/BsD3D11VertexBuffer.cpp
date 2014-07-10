//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D11VertexBuffer.h"
#include "BsD3D11Device.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	D3D11VertexBuffer::D3D11VertexBuffer(D3D11Device& device, UINT32 vertexSize, UINT32 numVertices, 
		GpuBufferUsage usage, bool useSystemMem, bool streamOut)
		:VertexBuffer(vertexSize, numVertices, usage, useSystemMem), mDevice(device), mStreamOut(streamOut)
	{ }

	D3D11VertexBuffer::~D3D11VertexBuffer()
	{ }

	void* D3D11VertexBuffer::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
#if BS_PROFILING_ENABLED
		if (options == GBL_READ_ONLY || options == GBL_READ_WRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_VertexBuffer);
		}

		if (options == GBL_READ_WRITE || options == GBL_WRITE_ONLY || options == GBL_WRITE_ONLY_DISCARD || options == GBL_WRITE_ONLY_NO_OVERWRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_VertexBuffer);
		}
#endif

		return mBuffer->lock(offset, length, options);
	}

	void D3D11VertexBuffer::unlockImpl()
	{
		mBuffer->unlock();
	}

	void D3D11VertexBuffer::readData(UINT32 offset, UINT32 length, void* dest)
	{
		mBuffer->readData(offset, length, dest);
		BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_VertexBuffer);
	}

	void D3D11VertexBuffer::writeData(UINT32 offset, UINT32 length, const void* source, BufferWriteType writeFlags)
	{
		mBuffer->writeData(offset, length, source, writeFlags);
		BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_VertexBuffer);
	}

	void D3D11VertexBuffer::copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
		mBuffer->copyData(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
	}

	void D3D11VertexBuffer::initialize_internal()
	{
		mBuffer = bs_new<D3D11HardwareBuffer, PoolAlloc>(D3D11HardwareBuffer::BT_VERTEX, 
			mUsage, 1, mSizeInBytes, std::ref(mDevice), mSystemMemory, mStreamOut);

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_VertexBuffer);
		VertexBuffer::initialize_internal();
	}

	void D3D11VertexBuffer::destroy_internal()
	{
		if(mBuffer != nullptr)
			bs_delete<PoolAlloc>(mBuffer);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_VertexBuffer);
		VertexBuffer::destroy_internal();
	}
}