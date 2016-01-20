//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsGLGpuBuffer.h"
#include "BsDebug.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	GLGpuBufferCore::GLGpuBufferCore(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
		: GpuBufferCore(elementCount, elementSize, type, usage, randomGpuWrite, useCounter)
	{
	}

	GLGpuBufferCore::~GLGpuBufferCore()
	{
		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuBuffer);
		clearBufferViews();
	}

	void GLGpuBufferCore::initialize()
	{
		LOGWRN("Generic buffers are not supported in OpenGL. Creating a dummy buffer. All operations on it will either be no-op or return a nullptr.");

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuBuffer);
		GpuBufferCore::initialize();
	}

	void* GLGpuBufferCore::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
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

	void GLGpuBufferCore::unlock()
	{
	}

	void GLGpuBufferCore::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_GpuBuffer);
	}

	void GLGpuBufferCore::writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags)
	{
		BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_GpuBuffer);
	}

	void GLGpuBufferCore::copyData(GpuBufferCore& srcBuffer, UINT32 srcOffset,
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
	}

	GpuBufferView* GLGpuBufferCore::createView()
	{
		return nullptr;
	}

	void GLGpuBufferCore::destroyView(GpuBufferView* view)
	{
	}
}