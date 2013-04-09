#include "CmGLGpuParamBlock.h"
#include "CmException.h"

namespace CamelotEngine
{
	GLGpuParamBlockBuffer::GLGpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage)
		:GpuParamBlockBuffer(size, usage)
	{
		glGenBuffers(1, &mGLHandle);
		glBindBuffer(GL_UNIFORM_BUFFER, mGLHandle);
		if(mUsage == GPBU_STATIC)
			glBufferData(GL_UNIFORM_BUFFER, mSize, (GLvoid*)mData, GL_STATIC_DRAW);
		else if(mUsage == GPBU_DYNAMIC)
			glBufferData(GL_UNIFORM_BUFFER, mSize, (GLvoid*)mData, GL_DYNAMIC_DRAW);
		else
			CM_EXCEPT(InternalErrorException, "Invalid gpu param block usage.");

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	GLGpuParamBlockBuffer::~GLGpuParamBlockBuffer()
	{
		glDeleteBuffers(1, &mGLHandle);
	}

	void GLGpuParamBlockBuffer::writeAll(const void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mGLHandle);
		glBufferSubData(GL_UNIFORM_BUFFER, 0 , mSize, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		GpuParamBlockBuffer::writeAll(data);
	}

	GpuParamBlockBuffer* GLGpuParamBlock::createBuffer() const
	{
		return CM_NEW(GLGpuParamBlockBuffer, PoolAlloc) GLGpuParamBlockBuffer(mSize, mUsage);
	}
}