//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGLGpuBuffer.h"
#include "BsDebug.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	GLGpuBuffer::GLGpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter) 
		: GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter)
	{
	}

	GLGpuBuffer::~GLGpuBuffer()
	{
	}

	void GLGpuBuffer::initialize_internal()
	{
		LOGWRN("Generic buffers are not supported in OpenGL. Creating a dummy buffer. All operations on it will either be no-op or return a nullptr.");

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuBuffer);
		GpuBuffer::initialize_internal();
	}

	void GLGpuBuffer::destroy_internal()
	{
		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuBuffer);
		GpuBuffer::destroy_internal();
	}

	void* GLGpuBuffer::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
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

		return nullptr;
	}

	void GLGpuBuffer::unlock()
	{
	}

	void GLGpuBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_GpuBuffer);
	}

	void GLGpuBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags)
	{
		BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_GpuBuffer);
	}

	void GLGpuBuffer::copyData(GpuBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
	}

	GpuBufferView* GLGpuBuffer::createView()
	{
		return nullptr;
	}

	void GLGpuBuffer::destroyView(GpuBufferView* view)
	{
	}
}