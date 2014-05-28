#include "CmGLGpuParamBlockBuffer.h"
#include "CmException.h"

namespace BansheeEngine
{
	GLGpuParamBlockBuffer::GLGpuParamBlockBuffer()
		:mGLHandle(0)
	{
	}

	void GLGpuParamBlockBuffer::initialize_internal()
	{
		glGenBuffers(1, &mGLHandle);
		glBindBuffer(GL_UNIFORM_BUFFER, mGLHandle);
		if(mUsage == GPBU_STATIC)
			glBufferData(GL_UNIFORM_BUFFER, mSize, nullptr, GL_STATIC_DRAW);
		else if(mUsage == GPBU_DYNAMIC)
			glBufferData(GL_UNIFORM_BUFFER, mSize, nullptr, GL_DYNAMIC_DRAW);
		else
			CM_EXCEPT(InternalErrorException, "Invalid gpu param block usage.");

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		GpuParamBlockBuffer::initialize_internal();
	}

	void GLGpuParamBlockBuffer::destroy_internal()
	{
		glDeleteBuffers(1, &mGLHandle);

		GpuParamBlockBuffer::destroy_internal();
	}

	void GLGpuParamBlockBuffer::writeData(const UINT8* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mGLHandle);
		glBufferSubData(GL_UNIFORM_BUFFER, 0 , mSize, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void GLGpuParamBlockBuffer::readData(UINT8* data) const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mGLHandle);
		glGetBufferSubData(GL_UNIFORM_BUFFER, 0 , mSize, (GLvoid*)data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}