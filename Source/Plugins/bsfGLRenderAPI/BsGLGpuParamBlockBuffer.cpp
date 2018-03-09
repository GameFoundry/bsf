//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsGLGpuParamBlockBuffer.h"
#include "Profiling/BsRenderStats.h"
#include "Error/BsException.h"

namespace bs { namespace ct
{
	GLGpuParamBlockBuffer::GLGpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage, GpuDeviceFlags deviceMask)
		:GpuParamBlockBuffer(size, usage, deviceMask), mGLHandle(0)
	{
		assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on OpenGL.");
	}

	GLGpuParamBlockBuffer::~GLGpuParamBlockBuffer()
	{
		glDeleteBuffers(1, &mGLHandle);
		BS_CHECK_GL_ERROR();

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuParamBuffer);
	}

	void GLGpuParamBlockBuffer::initialize()
	{
		glGenBuffers(1, &mGLHandle);
		BS_CHECK_GL_ERROR();

		glBindBuffer(GL_UNIFORM_BUFFER, mGLHandle);
		BS_CHECK_GL_ERROR();

		if (mUsage == GPBU_STATIC)
		{
			glBufferData(GL_UNIFORM_BUFFER, mSize, nullptr, GL_STATIC_DRAW);
			BS_CHECK_GL_ERROR();
		}
		else if (mUsage == GPBU_DYNAMIC)
		{
			glBufferData(GL_UNIFORM_BUFFER, mSize, nullptr, GL_DYNAMIC_DRAW);
			BS_CHECK_GL_ERROR();
		}
		else
			BS_EXCEPT(InternalErrorException, "Invalid gpu param block usage.");

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		BS_CHECK_GL_ERROR();

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuParamBuffer);
		GpuParamBlockBuffer::initialize();
	}

	void GLGpuParamBlockBuffer::writeToGPU(const UINT8* data, UINT32 queueIdx)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mGLHandle);
		BS_CHECK_GL_ERROR();

		glBufferSubData(GL_UNIFORM_BUFFER, 0 , mSize, data);
		BS_CHECK_GL_ERROR();

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		BS_CHECK_GL_ERROR();

		BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_GpuParamBuffer);
	}
}}